#include <xUnit++/xUnit++.h>
#include <entity/entity.h>
#include <entity/json.h>

using namespace std;
using namespace ent;


struct SubClass : entity
{
	string name	= "default";
	bool flag	= false;


	void create_map()
	{
		map("name", this->name);
		map("flag", this->flag);
	}
};

struct TestClass : entity
{
	string name;
	int integer;
	double floating;
	SubClass sub;

	vector<string> strings;
	vector<int> integers;
	vector<SubClass> objects;
	vector<byte> binary;
	//vector<vector<int>> complex;

	std::map<string, string> dictionary;
	std::map<string, SubClass> objectMap;

	
	void create_map()
	{
		map("name", this->name);
		map("integer", this->integer);
		map("floating", this->floating);
		map("sub", this->sub);
		map("strings", this->strings);
		map("integers", this->integers);
		map("objects", this->objects);
		map("binary", this->binary);
		//map("complex", this->complex);

		map("dictionary", this->dictionary);
		map("objectMap", this->objectMap);
	}
};


string js = u8R"json(
{
	"name":			"hello",
	"binary":		"AAECiP8=",
	"floating":		1.2,
	"integer":		42,
	"integers":		[ 1, 1, 2, 3, 5, 8 ],
	"strings": 		[ "array", "of", "strings" ],
	"dictionary":	{ "first": "1", "second": "2"	},

	"objectMap": {
		"first":	{ "flag": true, "name": "map item 1" },
		"second":	{ "flag": false, "name": "map item 2" }
	},

	"objects": [
		{ "flag": false, "name": "array item 1" },
		{ "flag": true, "name": "array item 2" }
	],

	"sub": {
		"flag":	true,
		"name":	"sub hello"
	}
}
")json";


#include <chrono>
using namespace chrono;
typedef duration<int, std::milli> ms;

SUITE("Entity Tests")
{
	FACT("Mapping does work")
	{
		/*tree t = tree()
			.set("a", map<string, string> {{ "a", "1" }, { "b", "2" }})
			.set("b", map<string, tree> {{ "a", tree().set("a", "1") }, { "b", tree().set("b", "2") }});

		cout << t.to<json>(true) << endl;

		for (auto &i : t.map<string>("a")) cout << i.first << "=" << i.second << ", ";
		cout << endl;

		for (auto &i : t.map<tree>("b")) cout << i.first << "=" << i.second.properties.size() << ", ";
		cout << endl;//*/

		TestClass tc;

		/*tc.name = "hello";
		tc.integer = 42;
		tc.floating = 1.2;
		tc.sub.name = "sub hello";
		tc.sub.flag = true;

		tc.strings = { "array", "of", "strings" };
		tc.integers = { 1, 1, 2, 3, 5, 8 };
		tc.objects = { tc.sub, tc.sub };
		tc.binary = { 0x00, 0x01, 0x02, 0x88, 0xff };

		tc.dictionary = { { "first", "1" }, { "second", "2" }};
		tc.objectMap = { { "first", tc.sub }, { "second", tc.sub }};

		cout << tc.to<json>(true) << endl; //*/

		tc.from<json>(js);

		cout << endl << "Scalar" << endl;
		cout << tc.name << endl;
		cout << tc.integer << endl;
		cout << tc.floating << endl;
		cout << tc.sub.to<json>() << endl;

		cout << endl << "Arrays" << endl;
		for (auto &i : tc.strings)	cout << i << ", ";		cout << endl;
		for (auto &i : tc.integers) cout << i << ", ";		cout << endl;
		for (auto &i : tc.objects)	cout << i.to<json>() << ", "; cout << endl;
		for (auto &i : tc.binary)	cout << (int)i << ", ";	cout << endl;

		cout << endl << "Maps" << endl;
		for (auto &i : tc.dictionary) 	cout << i.first << "=" << i.second << ", ";				cout << endl;
		for (auto &i : tc.objectMap)	cout << i.first << "=" << i.second.to<json>() << ", ";	cout << endl;

		/*
		 * Test to make sure mappings are refreshed when an entity is copy constructed

		TestClass a;
		a.from<json>(js);

		TestClass b = a;
		a.name = "Another name";
		cout << b.to<json>(true) << endl;*/
	}
}


