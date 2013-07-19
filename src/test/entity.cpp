#include <xUnit++/xUnit++.h>
#include <entity/entity.h>
#include <entity/json.h>

using namespace std;
using namespace ent;



struct SimpleEntity : entity
{
	string name = "simple";
	bool flag	= true;
	int integer	= 42;
	
	void create_map()
	{
		map("name", this->name);
		map("flag", this->flag);
		map("integer", this->integer);
	}
};



struct CollectionEntity : entity
{
	std::vector<string> strings	 			= { "one", "two", "three" };
	std::vector<double> doubles				= { 0.11, 0.22, 0.33 };
	std::vector<byte> binary				= { 0x00, 0x01, 0x02, 0x88, 0xff };
	std::map<string, string> dictionary		= { { "first", "item" }, { "second", "item" } };

	void create_map()
	{
		map("strings", this->strings);
		map("doubles", this->doubles);
		map("binary", this->binary);
		map("dictionary", this->dictionary);
	}
};



struct ComplexEntity : entity
{
	string name 						= "complex";
	std::vector<SimpleEntity> entities	= { SimpleEntity(), SimpleEntity() };
	CollectionEntity collection;
	SimpleEntity simple;

	void create_map()
	{
		map("name", this->name);
		map("entities", this->entities);
		map("collection", this->collection);
		map("simple", this->simple);
	}
};



SUITE("Entity Tests")
{
	FACT("A simple entity can be mapped to a tree")
	{
		tree t = SimpleEntity().to_tree();

		Assert.Equal("simple",	t.get<string>("name"));
		Assert.Equal(42,		t.get<int>("integer"));
		Assert.True(			t.get<bool>("flag"));
	}


	FACT("A simple entity can be mapped from a tree")
	{
		SimpleEntity e;
		e.from_tree(tree()
			.set("name", "changed")
			.set("integer", 1)
			.set("flag", false)
		);

		Assert.Equal("changed", e.name);
		Assert.Equal(1,			e.integer);
		Assert.False(			e.flag);
	}


	FACT("An entity with collections can be mapped to a tree")
	{
		tree t = CollectionEntity().to_tree();

		Assert.Equal(vector<string> { "one", "two", "three" },							t.array<string>("strings"));
		Assert.Equal(vector<double> { 0.11, 0.22, 0.33 },								t.array<double>("doubles"));
		Assert.Equal(vector<byte> { 0x00, 0x01, 0x02, 0x88, 0xff },						t.get<vector<byte>>("binary"));
		Assert.Equal(map<string, string> { { "first", "item" }, { "second", "item" } },	t.map<string>("dictionary"));
	}


	FACT("An entity with collections can be mapped from a tree")
	{
		CollectionEntity e;
		e.from_tree(tree()
			.set("strings", vector<string> { "four", "five" })
			.set("doubles", vector<double> { 5.0, 3.0, 1.0, 0.0 })
			.set("binary", vector<byte> { 0xff, 0x00 })
			.set("dictionary", map<string, string> { { "first", "changed" }})
		);

		Assert.Equal(vector<string> { "four", "five" },				e.strings);
		Assert.Equal(vector<double> { 5.0, 3.0, 1.0, 0.0 },			e.doubles);
		Assert.Equal(vector<byte> { 0xff, 0x00 },					e.binary);
		Assert.Equal(map<string, string> { { "first", "changed" }},	e.dictionary);
	}


	FACT("A complex entity can be mapped to a tree")
	{
		tree t = ComplexEntity().to_tree();

		Assert.Equal("complex",									t.get<string>("name"));
		Assert.Equal("simple", 									t.get("simple").get<string>("name"));
		Assert.Equal(vector<string> { "one", "two", "three" },	t.get("collection").array<string>("strings"));
		Assert.Equal(42,										t.array<tree>("entities")[0].get<int>("integer"));
	}


	FACT("A complex entity can be mapped from a tree")
	{
		ComplexEntity e;
		e.from_tree(tree()
			.set("name", "changed")
			.set("simple", tree().set("integer", 1024))
			.set("collection", tree().set("doubles", vector<double> { 42.42 }))
			.set("entities", vector<tree> { tree().set("name", "item") })
		);

		Assert.Equal("changed", e.name);
		Assert.Equal(1024,		e.simple.integer);
		Assert.Equal(1,			e.collection.doubles.size());
		Assert.Equal(42.42,		e.collection.doubles[0]);
		Assert.Equal(1,			e.entities.size());
		Assert.Equal("item",	e.entities[0].name);
	}


	FACT("An entity can be serialised")
	{
		Assert.Equal(u8R"json({"flag":true,"integer":42,"name":"simple"})json", SimpleEntity().to<json>());
	}


	FACT("An entity can be deserialised")
	{
		ComplexEntity e;

		e.from<json>(u8R"json({
			"name":	"parsed complex",
			"simple": {
				"name":		"parsed simple",
				"flag":		false,
				"integer":	1234
			},
			"collection": {
				"strings":		[ "a", "b", "c", "d" ],
				"doubles":		[ 9, 8, 7, 6 ],
				"binary":		"AAECiP8=",
				"dictionary":	{ "a": "1", "b": "2" }
			},
			"entities": [
				{ "name": "simple 1", "integer": 1 },
				{ "name": "simple 2", "integer": 2 },
			]
		})json");

		Assert.Equal("parsed complex",	e.name);
		Assert.Equal("parsed simple",	e.simple.name);
		Assert.Equal(1234,				e.simple.integer);
		Assert.Equal("b",				e.collection.strings[1]);
		Assert.Equal(7,					e.collection.doubles[2]);
		Assert.Equal(0x88,				e.collection.binary[3]);
		Assert.Equal("2",				e.collection.dictionary["b"]);
		Assert.Equal("simple 1",		e.entities[0].name);
		Assert.Equal(2,					e.entities[1].integer);
	}
}


