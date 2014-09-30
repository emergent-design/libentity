#include <xUnit++/xUnit++.h>
#include <entity/entity.hpp>
#include <entity/json.hpp>

#include "test/entities.h"


using namespace std;
using namespace ent;

// TODO: Tests mapping to vref?

SUITE("Entity Tests")
{


/*
	FACT("A simple entity can be mapped to a tree")
	{
		tree t = SimpleEntity().to();

		Assert.Equal("simple",	t.get<string>("name"));
		Assert.Equal(42,		t.get<int>("integer"));
		Assert.Equal(20349758,	t.get<long>("bignumber"));
		Assert.Equal(3.142,		t.get<double>("floating"));
		Assert.True(			t.get<bool>("flag"));
	}


	FACT("A simple entity can be mapped from a tree")
	{
		SimpleEntity e;
		e.from(tree()
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
		tree t = CollectionEntity().to();

		Assert.Equal(vector<string> { "one", "two", "three" },							t.array<string>("strings"));
		Assert.Equal(vector<double> { 0.11, 0.22, 0.33 },								t.array<double>("doubles"));
		Assert.Equal(vector<byte> { 0x00, 0x01, 0x02, 0x88, 0xff },						t.get<vector<byte>>("binary"));
		Assert.Equal(map<string, string> { { "first", "item" }, { "second", "item" } },	t.map<string>("dictionary"));
	}


	FACT("An entity with collections can be mapped from a tree")
	{
		CollectionEntity e;
		e.from(tree()
			.set("strings", vector<string> { "four", "five" })
			.set("doubles", vector<double> { 5.0, 3.0, 1.0, 0.0 })
			.set("binary", vector<byte> { 0xff, 0x00 })
			.set("dictionary", map<string, string> { { "first", "changed" }})
		);

		Assert.Equal(vector<string> { "four", "five" },				e.strings);
		Assert.Equal(vector<double> { 5.0, 3.0, 1.0, 0.0 },			e.doubles);
		Assert.Equal(vector<byte> { 0xff, 0x00 },					e.binary);
		Assert.Equal(map<string, string> {{ "first", "changed" }},	e.dictionary);
	}


	FACT("A complex entity can be mapped to a tree")
	{
		tree t = ComplexEntity().to();

		Assert.Equal("complex",									t.get<string>("name"));
		Assert.Equal("simple", 									t.get("simple").get<string>("name"));
		Assert.Equal(vector<string> { "one", "two", "three" },	t.get("collection").array<string>("strings"));
		Assert.Equal(42,										t.array<tree>("entities")[0].get<int>("integer"));
	}


	FACT("A complex entity can be mapped from a tree")
	{
		ComplexEntity e;
		e.from(tree()
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
		Assert.Equal(u8R"json({"bignumber":20349758,"flag":true,"floating":3.142000,"integer":42,"name":"simple"})json", SimpleEntity().to<json>());
		Assert.Equal(u8R"xml(<?xml version="1.0" encoding="UTF-8" standalone="yes"?><entity><bignumber value="20349758" /><flag value="true" /><floating value="3.142" /><integer value="42" /><name>simple</name></entity>)xml", SimpleEntity().to<xml>());
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
	}*/
}

