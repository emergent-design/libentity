#include <xUnit++/xUnit++.h>
#include <entity/entity.hpp>
#include <entity/json.hpp>

using namespace std;
using namespace ent;


struct SimpleEntity : ent::entity
{
	string name		= "simple";
	bool flag		= true;
	int integer		= 42;
	long bignumber	= 20349758;
	double floating	= 3.142;

	mapping describe()
	{
		return { eref(name), eref(flag), eref(integer), eref(bignumber), eref(floating) };
	}
};


struct CollectionEntity : ent::entity
{
	vector<string> strings;
	vector<double> doubles;
	vector<byte> binary;
	map<string, string> dictionary;

	mapping describe()
	{
		return { eref(strings), eref(doubles), eref(binary), eref(dictionary) };
	}
};


struct ComplexEntity : ent::entity
{
	string name;
	vector<SimpleEntity> entities;
	CollectionEntity collection;
	SimpleEntity simple;

	mapping describe()
	{
		return { eref(name), eref(entities), eref(collection), eref(simple) };
	}
};


SUITE("Entity Tests")
{
	FACT("An entity can be serialised")
	{
		SimpleEntity e;

		Assert.Equal(
			u8R"json({"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"})json",
			entity::encode<json>(e)
		);
	}


	FACT("An entity can be deserialised")
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
