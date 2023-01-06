#include "doctest.h"
#include <entity/query.hpp>
#include <ostream>
#include <cmath>

using namespace ent;
using std::string;

struct simple
{
	string name;
	int number;

	bool operator==(const simple &s) const
	{
		return this->name == s.name && this->number == s.number;
	}
};

std::ostream& operator<<(std::ostream& os, const simple &s)
{
	return os << "{" << s.name << "," << s.number << "}";
}


TEST_CASE("query brings LINQ-like features to STL containers") //, "[query]")
{
	std::vector<simple> objects	= {{ "a", 42 }, { "a", 8 }, { "b", 8 }, { "cb", 8 }, { "ca", 8 }};
	std::vector<string> letters	= { "a", "b", "c" };
	std::vector<string> strings	= { "the", "moon's", "a", "balloon" };
	std::vector<int> ints		= { 6, 3, 8, 2, 42, 4, 2, 6, 7 };
	std::vector<int> more_ints	= { 42, 100, 2, 3 };
	std::vector<int> empty;


	SUBCASE("can aggregate items")
	{
		auto accumulator	= [](const string &acc, const string &i) { return acc + ", " + i; };
		auto selector		= [](const string &i) { return i.size(); };

		CHECK(from(letters).aggregate(accumulator)								== "a, b, c");
		CHECK(from(letters).aggregate<string>("z", accumulator)					== "z, a, b, c");	// with seed
		CHECK((from(letters).aggregate<string, int>("z", accumulator, selector))	== 10);				// with seed and selector
	}


	SUBCASE("can aggregate an empty vector")
	{
		std::vector<string> empty;
		CHECK(from(empty).aggregate([](const string &acc, const string &i) { return acc + ", " + i; }) == "");
	}


	SUBCASE("can check if all items match the criteria")
	{
		CHECK(from(letters).all([](auto &i) { return !i.empty(); }));
		CHECK_FALSE(from(letters).all([](auto &i) { return i == "a"; }));
	}


	SUBCASE("can check if any items match the criteria")
	{
		CHECK(from(letters).any([](auto &i) { return i == "b"; }));
		CHECK_FALSE(from(letters).any([](auto &i) { return i.empty(); }));
	}


	SUBCASE("can calculate the average of numeric data")
	{
		CHECK(from(ints).average()												== doctest::Approx(8.8888));
		CHECK(from(ints).average<double>([](auto &i) { return std::sqrt(i); })	== doctest::Approx(2.6016));	// with selector
	}


	SUBCASE("can concatenate items")
	{
		CHECK(from(ints).concat(more_ints).count()			== 13);
		CHECK(from(ints).concat({ 42, 100, 1 }).count()		== 12);	// initialiser lists
		CHECK(from(ints).concat(from(more_ints)).count()	== 13);	// queries
	}


	SUBCASE("can check for item existence")
	{
		CHECK(from({ 1, 2, 3 }).contains(2));
		CHECK_FALSE(from({ 1, 2, 3 }).contains(4));
	}


	SUBCASE("can count items")
	{
		CHECK(from({ 1, 2, 3, 4, 5 }).count()								== 5);
		CHECK(from({ 1, 2, 3, 4, 5 }).count([](auto &i) { return i > 2; })	== 3);	// with predicate
	}


	SUBCASE("can return a default value if empty")
	{
		CHECK(from(empty).default_if_empty().element_at(0)		== 0);
		CHECK(from(empty).default_if_empty(42).element_at(0)	== 42);	// with specified default value
		CHECK(from({ 42 }).default_if_empty().element_at(0)		== 42);	// not empty
	}


	SUBCASE("can return items without duplicates")
	{
		CHECK(from(ints).distinct().vector() == std::vector<int>({ 6, 3, 8, 2, 42, 4, 7 }));
	}


	SUBCASE("can return the item at a given index")
	{
		CHECK(from(ints).element_at(2)				== 8);
		CHECK(from(ints).element_at_or_default(64)	== 0);	// with default if out of range
		CHECK_THROWS(from(ints).element_at(64));				// exception if out of range
	}


	SUBCASE("can return the set difference of two sequences")
	{
		CHECK(from(ints).except(more_ints).vector() == std::vector<int>({ 6, 8, 4, 6, 7 }));
	}


	SUBCASE("can return the first item")
	{
		CHECK(from(ints).first()											== 6);
		CHECK(from(ints).first([](auto &i) { return i < 5; })				== 3);	// with predicate
		CHECK(from(empty).first_or_default()								== 0);	// with default if sequence is empty
		CHECK(from(ints).first_or_default([](auto &i) { return i == 10; })	== 0);	// with default if predicate not met
		CHECK_THROWS(from(empty).first());											// exception if sequence is empty
		CHECK_THROWS(from(ints).first([](auto &i) { return i == 10; }));			// exception if predicate does not match
	}


	// SUBCASE("can group items by key") {}
	// SUBCASE("can join two sequences and then group items by key") {}
	// SUBCASE("can produce the intersect of two sequences") {}
	// SUBCASE("can join two sequences") {}


	SUBCASE("can return the last item")
	{
		CHECK(from(ints).last()												== 7);
		CHECK(from(ints).last([](auto &i) { return i < 5; })				== 2);	// with predicate
		CHECK(from(empty).last_or_default()									== 0);	// with default if sequence is empty
		CHECK(from(ints).last_or_default([](auto &i) { return i == 10; })	== 0);	// with default if predicate not met
		CHECK_THROWS(from(empty).last());											// exception if sequence is empty
		CHECK_THROWS(from(ints).last([](auto &i) { return i == 10; }));				// exception if predicate does not match
	}


	SUBCASE("can return the max value")
	{
		CHECK(from(ints).max()											== 42);	// without selector
		CHECK(from(strings).max<int>([](auto &i) { return i.size(); })	== 7);	// with selector (find the length of the longest string)
		CHECK_THROWS(from(empty).max());										// exception if sequence is empty
	}


	SUBCASE("can return the min value")
	{
		CHECK(from(ints).min()											== 2);	// without selector
		CHECK(from(strings).min<int>([](auto &i) { return i.size(); })	== 1);	// with selector (find the length of the shortest string)
		CHECK_THROWS(from(empty).min());										// exception if sequence is empty
	}


	SUBCASE("can order by selected key")
	{
		CHECK(from(strings).order_by<int>([](auto &i) { return i.size(); }).first()			== "a");	// ascending
		CHECK(from(strings).order_by<int>([](auto &i) { return i.size(); }, true).last()	== "a");	// descending
	}


	SUBCASE("can reverse the sequence")
	{
		CHECK(from(ints).reverse().first() == 7);
	}


	SUBCASE("can transform items in a sequence")
	{
		CHECK(from(strings).select<int>([](auto &i) { return i.size(); }).vector()			== std::vector<int>({ 3, 6, 1, 7 }));
		CHECK(from(strings).select<int>([](auto &i) { return i.size(); }).set().count(6)	== 1);
	}


	// SUBCASE("can transform items into sequences and join those sequences together") {} // select_many(selector)


	SUBCASE("can compare two sequences")
	{
		CHECK_FALSE(from(ints).sequence_equal(more_ints));
		CHECK(from(ints).sequence_equal(ints));
	}


	SUBCASE("can return a single item")
	{
		CHECK(from({ 42 }).single()											== 42);
		CHECK(from(ints).single([](auto &i) { return i > 8; })				== 42);	// with predicate
		CHECK(from(empty).single_or_default()								== 0);	// default value if sequence is empty
		CHECK(from(ints).single_or_default([](auto &i) { return i < 0; })	== 0);	// with predicate but no match
		CHECK_THROWS(from(empty).single());											// exception if sequence is empty
		CHECK_THROWS(from(ints).single());											// exception if sequence size > 1
		CHECK_THROWS(from(ints).single_or_default());								// exception if sequence size > 1
		CHECK_THROWS(from(ints).single_or_default([](auto &i) { return i < 4; }));	// exception with predicate but multiple matches
	}


	SUBCASE("can skip items")
	{
		CHECK(from(ints).skip(3).first()									== 2);	// first N
		CHECK(from(ints).skip_while([](auto &i) { return i < 10; }).count()	== 5);	// with predicate
		CHECK(from(ints).skip_while([](auto &)  { return true; }).count()	== 0);	// with predicate
	}


	SUBCASE("can sum items")
	{
		CHECK(from(ints).sum() == 80);
		CHECK(from(ints).sum<double>([](auto &i) { return sqrt(i); }) == doctest::Approx(23.4144));	// with selector
	}


	SUBCASE("can take items")
	{
		CHECK(from(ints).take(2).vector()									== std::vector<int>({ 6, 3 }));	// first N
		CHECK(from(ints).take_while([](auto &i) { return i < 10; }).count()	== 4);							// with predicate
	}


	SUBCASE("can order by multiple criteria")
	{
		auto expected 	= std::vector<simple> {{ "a", 8 }, { "b", 8 }, { "ca", 8 }, { "cb", 8 }, { "a", 42 }};
		auto items		= from(objects)
			.order_by<int>([](auto &i) { return i.number; })
			.then_by<string>([](auto &i) { return i.name; });

		CHECK(items.vector() == expected);

		// then_by chained with anything other than order_by has no effect
		CHECK(from(objects).then_by<string>([](auto &i) { return i.name; }).vector() == objects);
	}


	SUBCASE("can convert the resulting sequence to a container type")
	{
		auto expected	= std::map<string, int> {{ "a", 8 }, { "b", 8 }, { "ca", 8 }, { "cb", 8 }};
		auto items		= from(objects).map<string, int>(
			[](auto &i) { return i.name; },		// key selector
			[](auto &i) { return i.number; }	// value selector
		);

		CHECK(items == expected);
		CHECK(from({ 1, 2, 3}).vector()	== std::vector<int>({ 1, 2, 3 }));
		CHECK(from({ 1, 2, 3}).list()	== std::list<int>({ 1, 2, 3 }));
	}


	// SUBCASE("can return the union of two sequences") {}


	SUBCASE("can filter the sequence")
	{
		CHECK(from(ints).where([](auto &i) { return i > 10; }).count()					== 1);
		CHECK(from(ints).where([](auto &i) { return i % 2; }).count()					== 2);
		CHECK(from(objects).where([](auto &i) { return i.name.size() > 1; }).count()	== 2);
	}


	SUBCASE("can apply a transform to corresponding elements of two sequences")
	{
		auto items = from(ints).zip<int, int>(more_ints, [](auto &i, auto &j) { return i + j; });

		CHECK(items.vector() == std::vector<int>({ 48, 103, 10, 5 }));
	}


	SUBCASE("can chain many function calls")
	{
		auto items = from(objects)
			.select<int>([](auto &i) { return i.number; })
			.where([](auto &i) { return i < 10; })
			.skip(2);

		CHECK(items.vector() == std::vector<int>({ 8, 8 }));
	}


	SUBCASE("will execute the query only upon iteration or conversion to a container")
	{
		auto values = std::vector<double> { 10.0, 1.0, 3.14, 42.0, 0.0 };
		auto q		= from(values).where([](auto &i) { return i > 2.0; });

		// modify the vector
		values[0] = 0.1;
		values.push_back(1234.0);

		// execute the query
		CHECK(q.vector() == std::vector<double>({ 3.14, 42.0, 1234.0 }));
	}


	SUBCASE("can iterate with a range-based for")
	{
		int sum			= 0;
		auto predicate	= [](const int &i) { return i < 10; };

		for (auto &i : from(ints).where(predicate))
		{
			sum += i;
		}

		CHECK(sum == 38);
	}
}

