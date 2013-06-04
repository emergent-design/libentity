//#include <entity/old_entity.h>
#include <entity/entity.h>
#include <entity/json.h>

using namespace std;
using namespace ent;

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

	auto tree = entity::from<json>(test);
	//auto tree = entity::from<json>(js);
	//auto tree = entity::from<json>(jsc);
	/*auto tree = entity()
		.set("a", "a value")
		.set("b", string("b value"))
		.set("c", entity().set("a", "child value"))
		.set("d", vector<string> { "set", "of", "values" })
		.set("e", vector<entity> { entity().set("a", "another child value") })
		.set("f", 1)
		.set("g", 3.14)
		.set("h", true)
		.set("i", vector<byte> { 0x00, 0x01, 0x02, 0x88, 0xff }); //*/

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
	//cout << endl << tree.to<json>(true) << endl;
	return 0;
}
