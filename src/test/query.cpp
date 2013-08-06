#include <xUnit++/xUnit++.h>
#include <entity/entity.h>
#include <entity/query.h>
#include <entity/json.h>


using namespace std;
using namespace ent;



SUITE("Query Tests")
{
	struct QueryTest
	{
		string name;
		int number;
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

		vector<int> data = { 6, 3, 8, 2, 1 };
		vector<int> one = { 42 };
		vector<int> empty;


		/*for (auto &i : from(data).order_by<int>([](const int &i) { return i; }).vector()) cout << i << endl;

		cout << endl;
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
		for (auto &i : from(data).skip_while([](const int &i) { return i > 2; })) cout << i << endl;

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
