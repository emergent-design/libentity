#include <xUnit++/xUnit++.h>
// #include <entity/entity.h>
#include <entity/query.hpp>
// #include <entity/json.h>

#include <set>
#include <iostream>

using namespace std;
using namespace ent;



SUITE("Query Tests")
{
	struct QueryTest
	{
		string name;
		int number;
		double another;

		bool operator==(const QueryTest &a)
		{
			return this->name == a.name && this->number == a.number && this->another == a.another;
		}
	};

	FACT("A fact")
	{
		//auto r = setup();

		//for (auto &i : r.vector()) cout << i << endl;

		//vector<QueryTest> data = {{ "First", 1 }, { "Second", 2 }};


		//auto r = from(data).select<tree>([](QueryTest &i) { return tree().set("name", i.name).set("number", i.number); });


		//for (auto &i : r.vector()) cout << json::to(i, false) << endl;
		//auto r = from(data).map<string, int>([](QueryTest &i) { return i.name; }, [](QueryTest &i) { return i.number; });

		//for (auto &i : r) cout << i.first << " = " << i.second << endl;

		//auto t = from({ 1, 2, 3}).select<int>([](int &i) { return i * 2; });


		//vector<int> data = { 1, 1, 2, 3, 5 };

		cout << "Size: " << sizeof(query<int>) << endl << endl;

		vector<int> data = { 6, 3, 8, 2, 42, 4, 2, 6, 7 };
		vector<int> more = { 42, 100, 1 };
		vector<int> one = { 42 };
		vector<int> empty;

		vector<QueryTest> objects = {
			{ "a", 42, 	1.0 },
			{ "b", 4,	1.0 },
			{ "cc", 8 , 3.1 },
			{ "ca", 8, 	1.0 },
			{ "cc", 8, 1.0  },
			{ "ca", 8, 	1.0 },
			{ "d", 1, 1.0 },
		};


		for (auto &i : from(data).except(more)) cout << i << endl;


		//for (auto &i : from(data).concat(more)) cout << i << endl;
		//for (auto &i : from(data).concat(more).order_by<int>([](const int &i) { return i; })) cout << i << endl;

		//for (auto &i : from(data).concat(more)) 			cout << i << endl;
		//cout << endl;
		//for (auto &i : from(data).concat(more).reverse()) 	cout << i << endl;
		//cout << endl;
		//for (auto &i : from(data).concat(from(more))) 			cout << i << endl;

		//for (auto &i : from(data).list()) cout << i << endl;

		//for (auto &i : from(data).default_if_empty(10)) cout << i << endl;
		//cout << endl;
		//for (auto &i : from(empty).default_if_empty(10)) cout << i << endl;
		//cout << endl;
		//for (auto &i : from(empty).default_if_empty()) cout << i << endl;

		//for (auto &i : from(data).order_by<int>([](const int &i) { return i; }, true)) cout << i << endl;

		//auto items = from(objects)
		//	.order_by<int>([](const QueryTest &i) { return i.number; })
		//	.then_by<string>([](const QueryTest &i) { return i.name; })
		//	.then_by<double>([](const QueryTest &i) { return i.another; });

		//for (auto &i : items) cout << i.name << "," << i.number << endl;
		//for (auto &i : items) cout << i.name << "," << i.number << "," << i.another << endl;

		//cout << from(data).element_at(0) << endl;
		//cout << from(data).element_at_or_default(-1) << endl;

		//for (auto &i : from(data).distinct()) cout << i << endl;

		//for (auto &i : from(objects).distinct()) cout << i.name << endl;

		/*cout << endl;
		for (auto &i : from(data).where([](const int &i) { return i > 3; }).vector()) cout << i << endl;

		cout << endl;
		for (auto &i : from(data).select<string>([](const int &i) { return to_string(i) + "i"; }).vector()) cout << i << endl;

		cout << endl;
		for (auto &i : from(data).reverse().vector()) cout << i << endl;

		cout << endl;
		cout << from(data).first() << endl;
		cout << from(data).first([](const int &i) { return i < 5; }) << endl;
		cout << from(data).first_or_default([](const int &i) { return i == 10; }) << endl;

		cout << from(data).last() << endl;
		cout << from(data).last([](const int &i) { return i > 3; }) << endl;
		cout << from(data).last_or_default([](const int &i) { return i == 10; }) << endl;

		cout << endl;
		cout << from(data).count() << endl;
		cout << from(data).count([](const int &i) { return i > 3; }) << endl;*/

		//for (auto &i : from(data).where([](const int &i) { return i > 3; })) cout << i << endl;


		//cout << from(empty).single() << endl;
		//cout << from(data).single() << endl;
		//cout << from(one).single() << endl;
		//cout << from(empty).single_or_default() << endl;
		//cout << from(data).single([](const int &i) { return i > 6; }) << endl;
		//cout << from(data).single_or_default([](const int &i) { return i > 8; }) << endl;

		//for (auto &i : from(data).order_by<int>([](const int &i) { return i; }).take(3)) cout << i << endl;
		//for (auto &i : from(data).take_while([](const int &i) { return i > 1; })) cout << i << endl;
		//for (auto &i : from(empty).take(2)) cout << i << endl;

		//for (auto &i : from(data).skip(2)) cout << i << endl;
		//for (auto &i : from(data).skip_while([](const int &i) { return i > 2; })) cout << i << endl;

		//cout << from(data).select<int>([](const int &i) { return i * 2; }).min() << endl;

		//cout << from(data).min() << endl;
		//cout << from(data).min<double>([](const int &i) { return sqrt(i); }) << endl;

		//cout << from(data).max() << endl;
		//cout << from(data).max<double>([](const int &i) { return sqrt(i); }) << endl;

		//vector<string> strings = { "a", "b", "c" };

		//cout << from(strings).min() << endl;
		//cout << from(strings).average() << endl;
		//cout << from(data).average() << endl;
		//cout << from(data).average<double>([](const int &i) { return sqrt(i); }) << endl;


		//cout << from(strings).aggregate([](const string &accumulator, const string &i) { return accumulator + ", " + i; }) << endl;

		//cout << from(data).sum() << endl;
		//cout << from(data).sum<double>([](const int &i) { return sqrt(i); }) << endl;


		//int more[] = { 6, 3, 8 };

		//for (int &i : more) cout << i << endl;
		//cout << from(more).count() << endl;

		//wrapper<int> test(more, 3);

		//for (auto &i : from(more, 3)) cout << i << endl;



		//for (auto &i : from(data).where([](const int &i) { return i % 2; }).vector()) cout << i << endl;

		//auto r = from(data);

		//data.push_back(8);

		//cout << from(data).any([](int &i) { return i == 5; }) << endl;
		//cout << from(data).count([](int &i) { return i % 2; }) << endl;


		//map<string,int> data2 = { { "a", 1 }, { "b", 2 }};

		///auto r = from(data2);

		//for (auto &i : r.vector()) cout << i.first << "=" << i.second << endl;


		//cout << from({ 1, 2, 3 }).count() << endl;


		//cout << from(data).contains(3) << endl;

		//cout << from(data).first() << endl;
		//cout << from(data).last() << endl;
		//auto r = from(data).reverse();

		//for (auto &i : from(data).reverse().vector()) cout << i << endl;
		//for (auto &i : r.vector()) cout << i << endl;

		//vector<int> empty;

		//cout << from(empty).first() << endl;


		//auto r = from(data).where([](int &i) { return i > 2; }).vector();
		//auto r = from(data).vector();

		/*auto r = from(data)
			.where([](int &i) { return i % 2; })
			.select<string>([](int &i) { return to_string(i) + "i"; });
			//.vector();*/

		//auto r = from(data)
		//	.where([](int &i) { return i % 2; });
			//.where([](int &i) { return i > 1; });
	//		.select<string>([](int &i) { return to_string(i) + "i"; });
			//.vector(); */

		//auto r = from(data).map<int,string>([](int &i) { return i+1; }, [](int &i) { return to_string(i); });

		//for (auto &i : r) cout << i.first << " = " << i.second << endl;

		//cout << data.size() << endl;
		//cout << from(data).where([](int &i) { return i > 2; }).count() << endl;

		//cout << r.count() << endl;
		//for (auto &i : r.vector()) cout << i << endl;

		//for (auto &i : t.vector()) cout << i << endl;

		//cout << r.count() << endl;

		//for (auto &i : s) cout << i << ", ";
		//cout << endl;
	}
}
