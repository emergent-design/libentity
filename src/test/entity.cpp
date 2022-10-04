#include "catch.hpp"
#include <entity/entity.hpp>
#include <entity/json.hpp>

using namespace std;
using namespace ent;



struct SimpleEntity
{
	string name			= "simple";
	bool flag			= true;
	int integer			= 42;
	int64_t bignumber	= 20349758;
	double floating		= 3.142;

	emap(eref(name),eref(flag), eref(integer), eref(bignumber), eref(floating))
};


struct DerivedEntity : SimpleEntity
{
	string extra = "field";

	emerge(SimpleEntity, eref(extra))
};


struct CollectionEntity
{
	vector<string> strings;
	vector<double> doubles;
	vector<uint8_t> binary;
	set<int> ints;
	map<string, string> dictionary;

	emap(eref(strings), eref(doubles), eref(binary), eref(ints), eref(dictionary))
};


struct ComplexEntity
{
	string name;
	vector<SimpleEntity> entities;
	array<SimpleEntity, 4> extras;
	CollectionEntity collection;
	SimpleEntity simple;

	emap(eref(name), eref(entities), eref(extras), eref(collection), eref(simple))
};


struct TreeEntity
{
	string name;
	tree parameters;

	emap(eref(name), eref(parameters))
};


class ClassEntity
{
	public:
		emap(eref(name))

		string GetName() { return this->name; }

	private:
		string name = "class";
};


TEST_CASE("an entity can be serialised", "[entity]")
{
	SimpleEntity e;
	string data = R"json({"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"})json";

	REQUIRE(encode<json>(e) == data);
}


TEST_CASE("a derived entity can use a merge helper", "[entity]")
{
	DerivedEntity e;
	string data = R"json({"bignumber":20349758,"extra":"field","flag":true,"floating":3.142,"integer":42,"name":"simple"})json";

	REQUIRE(encode<json>(e) == data);
}


TEST_CASE("a vector of entities can be serialised", "[entity]")
{
	vector<SimpleEntity> e = { SimpleEntity() };
	string data = R"json([{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}])json";

	REQUIRE(encode<json>(e) == data);
}


TEST_CASE("an entity can be deserialised", "[entity]")
{
	auto e = decode<json, ComplexEntity>(R"json({
		"name":	"parsed complex",
		"simple": {
			"name":		"parsed simple",
			"flag":		false,
			"integer":	1234
		},
		"collection": {
			"strings":		[ "a", "b", "c", "d" ],
			"doubles":		[ 9, 8, 7, 6 ],
			"ints":			[ 2, 4, 8 ],
			"binary":		"AAECiP8=",
			"dictionary":	{ "a": "1", "b": "2" }
		},
		"entities": [
			{ "name": "simple 1", "integer": 1 },
			{ "name": "simple 2", "integer": 2 },
		],
		"extras": [
			{ "name": "extra 1", "integer": 4 }
		]
	})json");

	REQUIRE(e.name							== "parsed complex");
	REQUIRE(e.simple.name					== "parsed simple");
	REQUIRE(e.simple.integer				== 1234);
	REQUIRE(e.collection.strings[1]			== "b");
	REQUIRE(e.collection.doubles[2]			== 7);
	REQUIRE(e.collection.binary[3]			== 0x88);
	REQUIRE(e.collection.dictionary["b"]	== "2");
	REQUIRE(e.collection.ints.count(4)		== 1);
	REQUIRE(e.entities[0].name				== "simple 1");
	REQUIRE(e.entities[1].integer			== 2);
	REQUIRE(e.extras[0].name				== "extra 1");
	REQUIRE(e.extras[0].integer				== 4);
	REQUIRE(e.extras[1].name				== "simple");	// Unassigned array items are left with their original values
}


TEST_CASE("an entity can be converted to a tree", "[entity]")
{
	SimpleEntity e;
	auto t = to_tree(e);

	REQUIRE(t["name"].as_string() == "simple");
	REQUIRE(t["flag"].as_bool());
	REQUIRE(t["integer"].as_long() == 42);
	REQUIRE(t["bignumber"].as_long() == 20349758);
	REQUIRE(t["floating"].as_double() == 3.142);
}


TEST_CASE("an entity can be created from a tree", "[entity]")
{
	auto e = from_tree<ComplexEntity>({
		{ "name", "tree complex" },
		{ "simple", {
			{ "name", "tree simple" },
			{ "flag", false },
			{ "integer", 24816 }
		}},
		{ "collection", {
			{ "strings", vector<tree> { "z", "x", "y" }},
			{ "doubles", vector<tree> { 3.1, 4.1, 5.9 }},
			{ "binary", vector<uint8_t> { 0xaa, 0xbb, 0xcc }},
			{ "dictionary", { { "a", "1" }, { "b", "2" }}},
			{ "ints", vector<tree> { 2, 4, 8 }}
		}},
		{ "entities", vector<tree> {
			{{ "name", "simple 1" }, { "integer", 1 }},
			{{ "name", "simple 2" }, { "integer", 2 }}
		}},
		{ "extras", vector<tree> {
			{{ "name", "extra 1" }, { "integer", 4 }}
		}}
	});


	REQUIRE(e.name							== "tree complex");
	REQUIRE(e.simple.name					== "tree simple");
	REQUIRE(e.simple.integer				== 24816 );
	REQUIRE(e.collection.strings[1]			== "x");
	REQUIRE(e.collection.doubles[2]			== 5.9);
	REQUIRE(e.collection.binary[1]			== 0xbb);
	REQUIRE(e.collection.dictionary["b"]	== "2");
	REQUIRE(e.collection.ints.count(4)		== 1);
	REQUIRE(e.entities[0].name				== "simple 1");
	REQUIRE(e.entities[1].integer			== 2);
	REQUIRE(e.extras[0].name				== "extra 1");
	REQUIRE(e.extras[0].integer				== 4);
}


TEST_CASE("an entity can contain a tree and be serialised", "[entity]")
{
	TreeEntity e;
	e.name = "tree entity";
	e.parameters = tree().set("name", "parameter").set("flag", true);

	REQUIRE(encode<json>(e) == "{\"name\":\"tree entity\",\"parameters\":{\"flag\":true,\"name\":\"parameter\"}}");
}


TEST_CASE("an entity can contain a tree and be deserialised", "[entity]")
{
	auto e = decode<json, TreeEntity>(R"json({
		"name":	"tree entity",
		"parameters": {
			"name":		"parameter",
			"flag":		true,
			"integer":	1234
		}
	})json");

	REQUIRE(e.name == "tree entity");
	REQUIRE(e.parameters["name"] == "parameter");
	REQUIRE(e.parameters["flag"].as_bool());
	REQUIRE(e.parameters["integer"].as_long() == 1234);
}


TEST_CASE("an entity containing a tree can be converted to a tree", "[entity]")
{
	TreeEntity e;
	e.name = "tree entity";
	e.parameters = tree().set("name", "parameter").set("flag", true);

	auto t = to_tree(e);

	REQUIRE(t["name"] == "tree entity");
	REQUIRE(t["parameters"]["name"] == "parameter");
	REQUIRE(t["parameters"]["flag"].as_bool());
}


TEST_CASE("an entity containing a tree can be created from a tree", "[entity]")
{
	auto e = from_tree<TreeEntity>({
		{ "name", "tree entity" },
		{ "parameters", {
			{ "name", "parameter" },
			{ "flag", true },
			{ "integer", 1234 }
		}}
	});

	REQUIRE(e.name == "tree entity");
	REQUIRE(e.parameters["name"] == "parameter");
	REQUIRE(e.parameters["flag"].as_bool());
	REQUIRE(e.parameters["integer"].as_long() == 1234);
}


TEST_CASE("an entity can be decoded from JSON containing unused information", "[entity]")
{
	auto e = decode<json, ComplexEntity>(R"json({
		"entities":[{"a":{}}]
	})json");

	REQUIRE(e.entities.size() == 1);
}


TEST_CASE("a class with private members can be serialised", "[entity]")
{
	ClassEntity e;
	string data = R"json({"name":"class"})json";

	REQUIRE(encode<json>(e) == data);
}


TEST_CASE("a class with private members can be deserialised", "[entity]")
{
	auto e = decode<json, ClassEntity>(R"json({
		"name": "parsed class"
	})json");

	REQUIRE(e.GetName() == "parsed class");
}


TEST_CASE("entities should always revert to default values", "[entity]")
{
	auto e = decode<json, std::vector<SimpleEntity>>(R"json([
		{ "integer": 101 },
		{}
	])json");

	REQUIRE(e.front().integer == 101);
	REQUIRE(e.back().integer == 42);
}


TEST_CASE("maps of entities can be decoded from JSON", "[entity]")
{
	auto e = decode<json, std::map<string, SimpleEntity>>(R"json({
		"first": { "integer": 101 },
		"second": { "integer": 42 }
	})json");

	REQUIRE(e["first"].integer == 101);
	REQUIRE(e["second"].integer == 42);
}


TEST_CASE("entities with uint values lower than 64-bit can be handled")
{
	struct UnsignedEntity
	{
		uint16_t little = std::numeric_limits<uint16_t>::max();
		uint32_t medium = std::numeric_limits<uint32_t>::max();

		emap(eref(little), eref(medium))
	};
	UnsignedEntity uns;
	auto e = ent::to_tree(uns);

	REQUIRE(e["little"].as_long() == std::numeric_limits<uint16_t>::max());
	REQUIRE(e["medium"].as_long() == std::numeric_limits<uint32_t>::max());
}


// TEST_CASE("a terser mapping syntax can be used")
// {
// 	struct TerseEntity
// 	{
// 		string name			= "simple";
// 		bool flag			= true;
// 		int integer			= 42;
// 		double floating		= 3.142;

// 		ent_map_terse(name, flag, integer, floating)
// 	};
// }
