#include <entity/entity.h>
#include <entity/json.h>

using namespace std;
using namespace ent;

#include <chrono>
using namespace chrono;
typedef duration<int, std::milli> ms;
typedef duration<int, std::micro> us;
typedef duration<int, std::nano> ns;



/*map<string,string> tests = {
	{ "Support Empty Object", u8R"json({})json" },
	{ "Support Simple Object String value", u8R"json({ "v":"1"})json" },
	{ "Space Tester", "{	\"v\":\"1\"\r\n}" },
	{ "Support simple Object int value", u8R"json({ "v":1})json" },
	{ "Support simple Quote in String", u8R"json({ "v":"ab'c"})json" },
	{ "Support simple Object float value", u8R"json({ "v":3.141E-10})json" },
	{ "Support lowercase float value", u8R"json({ "v":3.141e-10})json" },
	{ "Long number support", u8R"json({ "v":12345123456789})json" },
	{ "Bigint number support", u8R"json({ "v":123456789123456789123456789})json" },
	//{ "Support simple digit array", u8R"json([ 1,2,3,4])json" },
	//{ "Support simple string array", u8R"json([ "1","2","3","4"])json" },
	//{ "Array of empty Object", u8R"json([ { }, { },[]])json" },
	{ "Support lowercase Unicode Text", u8R"json({ "v":"\u2000\u20ff"})json" },
	{ "Support uppercase Unicode Text", u8R"json({ "v":"\u2000\u20FF"})json" },
	{ "Support non protected / text", u8R"json({ "v":"hp://foo"})json" },
	{ "Support null", u8R"json({ "v":null})json" },
	{ "Support boolean", u8R"json({ "v":true})json" },
	{ "Support non trimmed data", u8R"json({ "v" : true })json" },
	{ "Double precision floating point", u8R"json({ "v":1.7976931348623157E308})json" },
};*/


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




string test_js = u8R"json(
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


int main(int argc, char *argv[])
{
	auto start = steady_clock::now();

	//std::chrono::nanoseconds

	TestClass tc;

	for (int i=0; i<1000; i++) tc.from<json>(test_js);

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

	cout << "Time = " << duration_cast<us>(steady_clock::now() - start).count() << "us" << endl;

	return 0;
}


/*
int main(int argc, char *argv[])
{
	string jsc = u8R"json({"a":"a value","b":"b value","c":{"a":"child value"},"d":[  "set",  "of",  "values"],"e":[  {"a":"another child value"}],"f":1,"g":3.14,"h":true,"i":"AAECiP8="}")json";
	string js = u8R"json(
		{
		  "a": "a value",
		  "b": "b value \"with\" quotes",
		  "c": {
			"a": "child value"
		  },
		  "f": 1,
		  "g": 3.14,
		  "h": true,
		  "i": "AAECiP8="
		  "d": [
			"set",
			"of",
			"values"
		  ],
		  "e": [
			{
			  "a": "another child value"
			}
		  ],
		  "j": [ 1.5, 5, 10.8, 5e2 ],
		  "k": [ 1, "string", { "a": "array child" }, [ "arrayarray" ], true, null ]
		}
	)json";

	string test = u8R"json(
		{
			"a": [
				{ "b" : "something" },
			]
		}
	)json";


	//for (auto &t : tests)
	//{
	//	cout << t.first << ": " << flush;
	//	auto tree = entity::from<json>(t.second);
	//	cout << tree.properties["v"] << endl;
	//}

	//auto tree = entity::from<json>(test);
	auto tree = tree::from<json>(js);
	//auto tree = entity::from<json>(jsc);
	//auto tree = entity()
	//	.set("a", "a value")
	//	.set("b", string("b value"))
	//	.set("c", entity().set("a", "child value"))
	//	.set("d", vector<string> { "set", "of", "values" })
	//	.set("e", vector<entity> { entity().set("a", "another child value") })
	//	.set("f", 1)
	//	.set("g", 3.14)
	//	.set("h", true)
	//	.set("i", vector<byte> { 0x00, 0x01, 0x02, 0x88, 0xff });

	cout << "a=" << tree.get<string>("a") << endl;
	cout << "b=" << tree.get<string>("b") << endl;
	cout << "c=" << tree.get("c").get<string>("a") << endl;
	cout << "f=" << tree.get<long>("f") << endl;
	cout << "g=" << tree.get<double>("g") << endl;
	cout << "h=" << tree.get<bool>("h") << endl;
	cout << "doesntexist=" << tree.get<long>("doesntexist", 42) << endl;
	cout << "wrongtype=" << tree.get<long>("a", 22) << endl;

	cout << "i= '" << tree.get<string>("i") << "' = ";
	for (auto &i : tree.get<vector<byte>>("i")) cout << (int)i << ", ";
	cout << endl;

	cout << "array: ";
	for (auto &i : tree.array<string>("d")) cout << i << ", ";
	cout << endl;

	cout << "objects: ";
	for (auto &i : tree.array("e")) cout << i.get<string>("a") << ", ";
	cout << endl;

	cout << "numbers: ";
	for (auto &i : tree.array<double>("j")) cout << i << ", ";
	cout << endl;

	cout << "allsorts: ";
	for (auto &i : tree.properties["k"].array) cout << (int)i.type << ", ";
	cout << endl;


	//cout << endl << tree.to<json>() << endl;
	cout << endl << tree.to<json>(true) << endl;
	return 0;
}*/

