#include "doctest.h"
#include <entity/entity.hpp>
#include <entity/json.hpp>

using namespace std;
using namespace ent;


TEST_SUITE("entity")
{
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


	TEST_CASE("an entity can be serialised")
	{
		SimpleEntity e;
		std::string_view data = R"json({"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"})json";

		CHECK(encode<json>(e) == data);
	}


	TEST_CASE("a const entity can be serialised")
	{
		std::string_view data = R"json({"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"})json";

		CHECK(encode<json>(SimpleEntity()) == data);
	}


	TEST_CASE("a derived entity can use a merge helper")
	{
		DerivedEntity e;
		std::string_view data = R"json({"bignumber":20349758,"extra":"field","flag":true,"floating":3.142,"integer":42,"name":"simple"})json";

		CHECK(encode<json>(e) == data);
	}

	TEST_CASE("a const derived entity can use a merge helper")
	{
		std::string_view data = R"json({"bignumber":20349758,"extra":"field","flag":true,"floating":3.142,"integer":42,"name":"simple"})json";

		CHECK(encode<json>(DerivedEntity()) == data);
	}


	TEST_CASE("a vector of entities can be serialised")
	{
		vector<SimpleEntity> e = { SimpleEntity() };
		std::string_view data = R"json([{"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"}])json";

		CHECK(encode<json>(e) == data);
	}


	TEST_CASE("an entity can be deserialised")
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

		CHECK(e.name						== "parsed complex");
		CHECK(e.simple.name					== "parsed simple");
		CHECK(e.simple.integer				== 1234);
		CHECK(e.collection.strings[1]		== "b");
		CHECK(e.collection.doubles[2]		== 7);
		CHECK(e.collection.binary[3]		== 0x88);
		CHECK(e.collection.dictionary["b"]	== "2");
		CHECK(e.collection.ints.count(4)	== 1);
		CHECK(e.entities[0].name			== "simple 1");
		CHECK(e.entities[1].integer			== 2);
		CHECK(e.extras[0].name				== "extra 1");
		CHECK(e.extras[0].integer			== 4);
		CHECK(e.extras[1].name				== "simple");	// Unassigned array items are left with their original values
	}


	TEST_CASE("an entity can be converted to a tree")
	{
		SimpleEntity e;
		auto t = to_tree(e);

		CHECK(t["name"].as_string() == "simple");
		CHECK(t["flag"].as_bool());
		CHECK(t["integer"].as_long() == 42);
		CHECK(t["bignumber"].as_long() == 20349758);
		CHECK(t["floating"].as_double() == 3.142);
	}

	TEST_CASE("a const entity can be converted to a tree")
	{
		auto t = to_tree<SimpleEntity>({});

		CHECK(t["name"].as_string() == "simple");
		CHECK(t["integer"].as_long() == 42);
	}


	TEST_CASE("an entity can be created from a tree")
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


		CHECK(e.name						== "tree complex");
		CHECK(e.simple.name					== "tree simple");
		CHECK(e.simple.integer				== 24816 );
		CHECK(e.collection.strings[1]		== "x");
		CHECK(e.collection.doubles[2]		== 5.9);
		CHECK(e.collection.binary[1]		== 0xbb);
		CHECK(e.collection.dictionary["b"]	== "2");
		CHECK(e.collection.ints.count(4)	== 1);
		CHECK(e.entities[0].name			== "simple 1");
		CHECK(e.entities[1].integer			== 2);
		CHECK(e.extras[0].name				== "extra 1");
		CHECK(e.extras[0].integer			== 4);
	}


	TEST_CASE("an entity can contain a tree and be serialised")
	{
		TreeEntity e;
		e.name = "tree entity";
		e.parameters = tree().set("name", "parameter").set("flag", true);

		CHECK(encode<json>(e) == R"json({"name":"tree entity","parameters":{"flag":true,"name":"parameter"}})json");
	}

	TEST_CASE("an const entity can contain a tree and be serialised")
	{
		const auto result = encode<json, TreeEntity>({
			"tree entity",
			tree().set("name", "parameter").set("flag", true)
		});

		CHECK(result == R"json({"name":"tree entity","parameters":{"flag":true,"name":"parameter"}})json");
	}


	TEST_CASE("an entity can contain a tree and be deserialised")
	{
		auto e = decode<json, TreeEntity>(R"json({
			"name":	"tree entity",
			"parameters": {
				"name":		"parameter",
				"flag":		true,
				"integer":	1234
			}
		})json");

		CHECK(e.name == "tree entity");
		CHECK(e.parameters["name"] == "parameter");
		CHECK(e.parameters["flag"].as_bool());
		CHECK(e.parameters["integer"].as_long() == 1234);
	}


	TEST_CASE("an entity containing a tree can be converted to a tree")
	{
		TreeEntity e;
		e.name			= "tree entity";
		e.parameters	= tree().set("name", "parameter").set("flag", true);
		auto t			= to_tree(e);

		CHECK(t["name"] == "tree entity");
		CHECK(t["parameters"]["name"] == "parameter");
		CHECK(t["parameters"]["flag"].as_bool());
	}


	TEST_CASE("a const entity containing a tree can be converted to a tree")
	{
		auto t = to_tree<TreeEntity>({
			"tree entity",
			tree().set("name", "parameter").set("flag", true)
		});

		CHECK(t["name"] == "tree entity");
		CHECK(t["parameters"]["name"] == "parameter");
		CHECK(t["parameters"]["flag"].as_bool());
	}


	TEST_CASE("an entity containing a tree can be created from a tree")
	{
		auto e = from_tree<TreeEntity>({
			{ "name", "tree entity" },
			{ "parameters", {
				{ "name", "parameter" },
				{ "flag", true },
				{ "integer", 1234 }
			}}
		});

		CHECK(e.name == "tree entity");
		CHECK(e.parameters["name"] == "parameter");
		CHECK(e.parameters["flag"].as_bool());
		CHECK(e.parameters["integer"].as_long() == 1234);
	}


	TEST_CASE("an entity can be decoded from JSON containing unused information")
	{
		auto e = decode<json, ComplexEntity>(R"json({
			"entities":[{"a":{}}]
		})json");

		CHECK(e.entities.size() == 1);
	}


	TEST_CASE("a class with private members can be serialised")
	{
		ClassEntity e;
		string data = R"json({"name":"class"})json";

		CHECK(encode<json>(e) == data);
	}


	TEST_CASE("a class with private members can be deserialised")
	{
		auto e = decode<json, ClassEntity>(R"json({
			"name": "parsed class"
		})json");

		CHECK(e.GetName() == "parsed class");
	}


	TEST_CASE("entities should always revert to default values")
	{
		auto e = decode<json, std::vector<SimpleEntity>>(R"json([
			{ "integer": 101 },
			{}
		])json");

		CHECK(e.front().integer == 101);
		CHECK(e.back().integer == 42);
	}


	TEST_CASE("maps of entities can be decoded from JSON")
	{
		auto e = decode<json, std::map<string, SimpleEntity>>(R"json({
			"first": { "integer": 101 },
			"second": { "integer": 42 }
		})json");

		CHECK(e["first"].integer == 101);
		CHECK(e["second"].integer == 42);
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

		CHECK(e["little"].as_long() == std::numeric_limits<uint16_t>::max());
		CHECK(e["medium"].as_long() == std::numeric_limits<uint32_t>::max());
	}


	#if __cplusplus >= 202002L

	TEST_CASE("a terser mapping syntax can be used")
	{
		struct TerseEntity
		{
			string name			= "simple";
			bool flag			= true;
			int integer			= 42;
			double floating		= 3.142;

			edesc(name, flag, integer, floating)
		};

		CHECK(ent::encode<ent::json>(TerseEntity()) == R"json({"flag":true,"floating":3.142,"integer":42,"name":"simple"})json");

		const auto t = ent::decode<ent::json, TerseEntity>(R"json({"flag":false,"floating":1.2,"integer":8,"name":"other"})json");

		CHECK(t.flag == false);
		CHECK(t.floating == 1.2);
		CHECK(t.integer == 8);
		CHECK(t.name == "other");
	}

	#endif

}
