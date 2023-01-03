#include "catch.hpp"
#include <entity/query.hpp>

using namespace std;
using namespace ent;


struct simple
{
	string name;
	int number;

	bool operator==(const simple &s) const
	{
		return this->name == s.name && this->number == s.number;
	}
};

ostream& operator<<(ostream& os, const simple &s)
{
	return os << "{" << s.name << "," << s.number << "}";
}


TEST_CASE("query brings LINQ-like features to STL containers", "[query]")
{
	vector<simple> objects	= {{ "a", 42 }, { "a", 8 }, { "b", 8 }, { "cb", 8 }, { "ca", 8 }};
	vector<string> letters	= { "a", "b", "c" };
	vector<string> strings	= { "the", "moon's", "a", "balloon" };
	vector<int> ints		= { 6, 3, 8, 2, 42, 4, 2, 6, 7 };
	vector<int> more_ints	= { 42, 100, 2, 3 };
	vector<int> empty;


	SECTION("can aggregate items")
	{
		auto accumulator	= [](const string &acc, const string &i) { return acc + ", " + i; };
		auto selector		= [](const string &i) { return i.size(); };

		REQUIRE(from(letters).aggregate(accumulator)								== "a, b, c");
		REQUIRE(from(letters).aggregate<string>("z", accumulator)					== "z, a, b, c");	// with seed
		REQUIRE((from(letters).aggregate<string, int>("z", accumulator, selector))	== 10);				// with seed and selector
	}


	SECTION("can aggregate an empty vector")
	{
		vector<string> empty;
		REQUIRE(from(empty).aggregate([](const string &acc, const string &i) { return acc + ", " + i; }) == "");
	}


	SECTION("can check if all items match the criteria")
	{
		REQUIRE(from(letters).all([](auto &i) { return !i.empty(); }));
		REQUIRE_FALSE(from(letters).all([](auto &i) { return i == "a"; }));
	}


	SECTION("can check if any items match the criteria")
	{
		REQUIRE(from(letters).any([](auto &i) { return i == "b"; }));
		REQUIRE_FALSE(from(letters).any([](auto &i) { return i.empty(); }));
	}


	SECTION("can calculate the average of numeric data")
	{
		REQUIRE(from(ints).average()										== Approx(8.8888));
		REQUIRE(from(ints).average<double>([](auto &i) { return sqrt(i); })	== Approx(2.6016));	// with selector
	}


	SECTION("can concatenate items")
	{
		REQUIRE(from(ints).concat(more_ints).count()		== 13);
		REQUIRE(from(ints).concat({ 42, 100, 1 }).count()	== 12);	// initialiser lists
		REQUIRE(from(ints).concat(from(more_ints)).count()	== 13);	// queries
	}


	SECTION("can check for item existence")
	{
		REQUIRE(from({ 1, 2, 3 }).contains(2));
		REQUIRE_FALSE(from({ 1, 2, 3 }).contains(4));
	}


	SECTION("can count items")
	{
		REQUIRE(from({ 1, 2, 3, 4, 5 }).count()									== 5);
		REQUIRE(from({ 1, 2, 3, 4, 5 }).count([](auto &i) { return i > 2; })	== 3);	// with predicate
	}


	SECTION("can return a default value if empty")
	{
		REQUIRE(from(empty).default_if_empty().element_at(0)	== 0);
		REQUIRE(from(empty).default_if_empty(42).element_at(0)	== 42);	// with specified default value
		REQUIRE(from({ 42 }).default_if_empty().element_at(0)	== 42);	// not empty
	}


	SECTION("can return items without duplicates")
	{
		REQUIRE(from(ints).distinct().vector() == vector<int>({ 6, 3, 8, 2, 42, 4, 7 }));
	}


	SECTION("can return the item at a given index")
	{
		REQUIRE(from(ints).element_at(2)				== 8);
		REQUIRE(from(ints).element_at_or_default(64)	== 0);	// with default if out of range
		REQUIRE_THROWS(from(ints).element_at(64));				// exception if out of range
	}


	SECTION("can return the set difference of two sequences")
	{
		REQUIRE(from(ints).except(more_ints).vector() == vector<int>({ 6, 8, 4, 6, 7 }));
	}


	SECTION("can return the first item")
	{
		REQUIRE(from(ints).first()												== 6);
		REQUIRE(from(ints).first([](auto &i) { return i < 5; })					== 3);	// with predicate
		REQUIRE(from(empty).first_or_default()									== 0);	// with default if sequence is empty
		REQUIRE(from(ints).first_or_default([](auto &i) { return i == 10; })	== 0);	// with default if predicate not met
		REQUIRE_THROWS(from(empty).first());											// exception if sequence is empty
		REQUIRE_THROWS(from(ints).first([](auto &i) { return i == 10; }));				// exception if predicate does not match
	}


	// SECTION("can group items by key") {}
	// SECTION("can join two sequences and then group items by key") {}
	// SECTION("can produce the intersect of two sequences") {}
	// SECTION("can join two sequences") {}


	SECTION("can return the last item")
	{
		REQUIRE(from(ints).last()											== 7);
		REQUIRE(from(ints).last([](auto &i) { return i < 5; })				== 2);	// with predicate
		REQUIRE(from(empty).last_or_default()								== 0);	// with default if sequence is empty
		REQUIRE(from(ints).last_or_default([](auto &i) { return i == 10; })	== 0);	// with default if predicate not met
		REQUIRE_THROWS(from(empty).last());											// exception if sequence is empty
		REQUIRE_THROWS(from(ints).last([](auto &i) { return i == 10; }));			// exception if predicate does not match
	}


	SECTION("can return the max value")
	{
		REQUIRE(from(ints).max()											== 42);	// without selector
		REQUIRE(from(strings).max<int>([](auto &i) { return i.size(); })	== 7);	// with selector (find the length of the longest string)
		REQUIRE_THROWS(from(empty).max());											// exception if sequence is empty
	}


	SECTION("can return the min value")
	{
		REQUIRE(from(ints).min()											== 2);	// without selector
		REQUIRE(from(strings).min<int>([](auto &i) { return i.size(); })	== 1);	// with selector (find the length of the shortest string)
		REQUIRE_THROWS(from(empty).min());											// exception if sequence is empty
	}


	SECTION("can order by selected key")
	{
		REQUIRE(from(strings).order_by<int>([](auto &i) { return i.size(); }).first()		== "a");	// ascending
		REQUIRE(from(strings).order_by<int>([](auto &i) { return i.size(); }, true).last()	== "a");	// descending
	}


	SECTION("can reverse the sequence")
	{
		REQUIRE(from(ints).reverse().first() == 7);
	}


	SECTION("can transform items in a sequence")
	{
		REQUIRE(from(strings).select<int>([](auto &i) { return i.size(); }).vector() == vector<int>({ 3, 6, 1, 7 }));
		REQUIRE(from(strings).select<int>([](auto &i) { return i.size(); }).set().count(6) == 1);
	}


	// SECTION("can transform items into sequences and join those sequences together") {} // select_many(selector)


	SECTION("can compare two sequences")
	{
		REQUIRE_FALSE(from(ints).sequence_equal(more_ints));
		REQUIRE(from(ints).sequence_equal(ints));
	}


	SECTION("can return a single item")
	{
		REQUIRE(from({ 42 }).single()										== 42);
		REQUIRE(from(ints).single([](auto &i) { return i > 8; })			== 42);		// with predicate
		REQUIRE(from(empty).single_or_default()								== 0);		// default value if sequence is empty
		REQUIRE(from(ints).single_or_default([](auto &i) { return i < 0; })	== 0);		// with predicate but no match
		REQUIRE_THROWS(from(empty).single());											// exception if sequence is empty
		REQUIRE_THROWS(from(ints).single());											// exception if sequence size > 1
		REQUIRE_THROWS(from(ints).single_or_default());									// exception if sequence size > 1
		REQUIRE_THROWS(from(ints).single_or_default([](auto &i) { return i < 4; }));	// exception with predicate but multiple matches
	}


	SECTION("can skip items")
	{
		REQUIRE(from(ints).skip(3).first()										== 2);	// first N
		REQUIRE(from(ints).skip_while([](auto &i) { return i < 10; }).count()	== 5);	// with predicate
		REQUIRE(from(ints).skip_while([](auto &)  { return true; }).count()		== 0);	// with predicate
	}


	SECTION("can sum items")
	{
		REQUIRE(from(ints).sum() == 80);
		REQUIRE(from(ints).sum<double>([](auto &i) { return sqrt(i); }) == Approx(23.4144));	// with selector
	}


	SECTION("can take items")
	{
		REQUIRE(from(ints).take(2).vector()										== vector<int>({ 6, 3 }));	// first N
		REQUIRE(from(ints).take_while([](auto &i) { return i < 10; }).count()	== 4);						// with predicate
	}


	SECTION("can order by multiple criteria")
	{
		auto expected 	= vector<simple> {{ "a", 8 }, { "b", 8 }, { "ca", 8 }, { "cb", 8 }, { "a", 42 }};
		auto items		= from(objects)
			.order_by<int>([](auto &i) { return i.number; })
			.then_by<string>([](auto &i) { return i.name; });

		REQUIRE(items.vector() == expected);

		// then_by chained with anything other than order_by has no effect
		REQUIRE(from(objects).then_by<string>([](auto &i) { return i.name; }).vector() == objects);
	}


	SECTION("can convert the resulting sequence to a container type")
	{
		auto expected	= map<string, int> {{ "a", 8 }, { "b", 8 }, { "ca", 8 }, { "cb", 8 }};
		auto items		= from(objects).map<string, int>(
			[](auto &i) { return i.name; },		// key selector
			[](auto &i) { return i.number; }	// value selector
		);

		REQUIRE(items == expected);
		REQUIRE(from({ 1, 2, 3}).vector()	== vector<int>({ 1, 2, 3 }));
		REQUIRE(from({ 1, 2, 3}).list()		== list<int>({ 1, 2, 3 }));
	}


	// SECTION("can return the union of two sequences") {}


	SECTION("can filter the sequence")
	{
		REQUIRE(from(ints).where([](auto &i) { return i > 10; }).count()				== 1);
		REQUIRE(from(ints).where([](auto &i) { return i % 2; }).count()					== 2);
		REQUIRE(from(objects).where([](auto &i) { return i.name.size() > 1; }).count()	== 2);
	}


	SECTION("can apply a transform to corresponding elements of two sequences")
	{
		auto items = from(ints).zip<int, int>(more_ints, [](auto &i, auto &j) { return i + j; });

		REQUIRE(items.vector() == vector<int>({ 48, 103, 10, 5 }));
	}


	SECTION("can chain many function calls")
	{
		auto items = from(objects)
			.select<int>([](auto &i) { return i.number; })
			.where([](auto &i) { return i < 10; })
			.skip(2);

		REQUIRE(items.vector() == vector<int>({ 8, 8 }));
	}


	SECTION("will execute the query only upon iteration or conversion to a container")
	{
		auto values = vector<double> { 10.0, 1.0, 3.14, 42.0, 0.0 };
		auto q		= from(values).where([](auto &i) { return i > 2.0; });

		// modify the vector
		values[0] = 0.1;
		values.push_back(1234.0);

		// execute the query
		REQUIRE(q.vector() == vector<double>({ 3.14, 42.0, 1234.0 }));
	}


	SECTION("can iterate with a range-based for")
	{
		int sum			= 0;
		auto predicate	= [](const int &i) { return i < 10; };

		for (auto &i : from(ints).where(predicate))
		{
			sum += i;
		}

		REQUIRE(sum == 38);
	}
}

