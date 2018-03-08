#include "catch.hpp"
#include <entity/entity.hpp>
#include <entity/json.hpp>

using namespace std;
using namespace ent;


struct SimpleEntity : ent::entity
{
	string name			= "simple";
	bool flag			= true;
	int integer			= 42;
	int64_t bignumber	= 20349758;
	double floating		= 3.142;

	mapping describe()
	{
		return { eref(name), eref(flag), eref(integer), eref(bignumber), eref(floating) };
	}
};


struct CollectionEntity : ent::entity
{
	vector<string> strings;
	vector<double> doubles;
	vector<uint8_t> binary;
	set<long> longs;
	map<string, string> dictionary;

	mapping describe()
	{
		return { eref(strings), eref(doubles), eref(binary), eref(longs), eref(dictionary) };
	}
};


struct ComplexEntity : ent::entity
{
	string name;
	vector<SimpleEntity> entities;
	array<SimpleEntity, 4> extras;
	CollectionEntity collection;
	SimpleEntity simple;

	mapping describe()
	{
		return { eref(name), eref(entities), eref(extras), eref(collection), eref(simple) };
	}
};


struct TreeEntity : ent::entity
{
	string name;
	tree parameters;

	mapping describe()
	{
		return { eref(name), eref(parameters) };
	}
};


TEST_CASE("an entity can be serialised", "[entity]")
{
	SimpleEntity e;
	string data = u8R"json({"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"})json";

	REQUIRE(entity::encode<json>(e) == data);
}


TEST_CASE("a vector of entities can be serialised", "[entity]")
{
	vector<SimpleEntity> e = { SimpleEntity() };
	string data = u8R"json([{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}])json";

	REQUIRE(entity::encode<json>(e) == data);
}


TEST_CASE("an entity can be deserialised", "[entity]")
{
	auto e = entity::decode<json, ComplexEntity>(u8R"json({
		"name":	"parsed complex",
		"simple": {
			"name":		"parsed simple",
			"flag":		false,
			"integer":	1234
		},
		"collection": {
			"strings":		[ "a", "b", "c", "d" ],
			"doubles":		[ 9, 8, 7, 6 ],
			"longs":		[ 2, 4, 8 ],
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
	REQUIRE(e.collection.longs.count(4)		== 1);
	REQUIRE(e.entities[0].name				== "simple 1");
	REQUIRE(e.entities[1].integer			== 2);
	REQUIRE(e.extras[0].name				== "extra 1");
	REQUIRE(e.extras[0].integer				== 4);
	REQUIRE(e.extras[1].name				== "simple");	// Unassigned array items are left with their original values
}


TEST_CASE("an entity can be converted to a tree", "[entity]")
{
	SimpleEntity e;
	auto t = entity::to_tree(e);

	REQUIRE(t["name"].as_string() == "simple");
	REQUIRE(t["flag"].as_bool());
	REQUIRE(t["integer"].as_long() == 42);
	REQUIRE(t["bignumber"].as_long() == 20349758);
	REQUIRE(t["floating"].as_double() == 3.142);
}


TEST_CASE("an entity can be created from a tree", "[entity]")
{
	auto e = entity::from_tree<ComplexEntity>({
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
			{ "longs", vector<tree> { 2, 4, 8 }}
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
	REQUIRE(e.collection.longs.count(4)		== 1);
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

	REQUIRE(entity::encode<json>(e) == "{\"name\":\"tree entity\",\"parameters\":{\"flag\":true,\"name\":\"parameter\"}}");
}


TEST_CASE("an entity can contain a tree and be deserialised", "[entity]")
{
	auto e = entity::decode<json, TreeEntity>(u8R"json({
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

	auto t = entity::to_tree(e);

	REQUIRE(t["name"] == "tree entity");
	REQUIRE(t["parameters"]["name"] == "parameter");
	REQUIRE(t["parameters"]["flag"].as_bool());
}


TEST_CASE("an entity containing a tree can be created from a tree", "[entity]")
{
	auto e = entity::from_tree<TreeEntity>({
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
	auto e = entity::decode<json, ComplexEntity>(u8R"json({
		"entities":[{"a":{}}]
	})json");

	REQUIRE(e.entities.size() == 1);
}
