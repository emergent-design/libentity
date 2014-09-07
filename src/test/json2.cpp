#include <xUnit++/xUnit++.h>
#include <test/entities.h>
#include <entity/json2.h>

using namespace std;
using namespace ent;


const string COMPACT_JSON	= u8R"json({"bignumber":20349758,"flag":true,"floating":3.142,"integer":42,"name":"simple"})json";
const string ALT_JSON		= u8R"json({"bignumber":12345678,"flag":false,"floating":1.414,"integer":3,"name":"alternative"})json";
const string PADDED_JSON	= u8R"json({
  "bignumber": 20349758,
  "flag": true,
  "floating": 3.142,
  "integer": 42,
  "name": "simple"
})json";

const string COLLECTION_JSON = u8R"json({
	"strings":		[ "one", "two", "three" ],
	"doubles":		[ 0.11, 0.22, 0.33 ],
	"binary":		"AAECiP8=",
	"dictionary":	{ "first": "item", "second": "item" }
})json";

const string COMPLEX_JSON = u8R"json({
	"collection":{
		"binary":"AAECiP8=",
		"dictionary":{
			"first":"item",
			"second":"item"
		},
		"doubles":[0.11,0.22,0.33],
		"strings":["one","two","three"]
	},
	"entities":[
		{"bignumber":12345678,"flag":false,"floating":1.414,"integer":3,"name":"alternative"},
		{"bignumber":12345678,"flag":false,"floating":1.414,"integer":3,"name":"alternative"}
	],
	"name":"complex",
	"simple":{"bignumber":12345678,"flag":false,"floating":1.414,"integer":3,"name":"alternative"}
})json";



SUITE("JSON Tests")
{
	FACT("Compact JSON can be generated")
	{
		SimpleEntity e;
		Assert.Equal(COMPACT_JSON, encode<json2>(e));
	}


	FACT("Compact JSON can be parsed")
	{
		auto e = decode<json2, SimpleEntity>(ALT_JSON);

		Assert.Equal(12345678, e.bignumber);
		Assert.False(e.flag);
		Assert.Equal(1.414, e.floating);
		Assert.Equal(3, e.integer);
		Assert.Equal("alternative", e.name);
	}


	FACT("Padded JSON can be generated")
	{
		SimpleEntity e;
		Assert.Equal(PADDED_JSON, encode<prettyjson>(e));
	}


	FACT("Padded JSON can be parsed")
	{
		auto e = decode<json2, SimpleEntity>(PADDED_JSON);

		Assert.Equal(20349758, e.bignumber);
		Assert.True(e.flag);
		Assert.Equal(3.142, e.floating);
		Assert.Equal(42, e.integer);
		Assert.Equal("simple", e.name);
	}


	FACT("Strings are escaped appropriately")
	{
		TextEntity e;
		e.text = "Must\tbe \"escaped\"\n";

		Assert.Equal(u8R"json({"text":"Must\tbe \"escaped\"\n"})json", encode<json2>(e));
	}


	FACT("Strings are unescaped appropriately")
	{
		auto e = decode<json2, TextEntity>(u8R"json({ "text": "Must\tbe \"escaped\"\n" })json");

		Assert.Equal("Must\tbe \"escaped\"\n", e.text);
	}


	FACT("Collections can be parsed")
	{
		auto e = decode<json2, CollectionEntity>(COLLECTION_JSON);

		Assert.Equal(3, e.strings.size());
		Assert.Equal("two", e.strings[1]);
		Assert.Equal(3, e.doubles.size());
		Assert.Equal(0.11, e.doubles[0]);
		Assert.Equal(vector<byte> { 0x00, 0x01, 0x02, 0x88, 0xff }, e.binary);
		Assert.True(e.dictionary.count("first") > 0);
		Assert.Equal("item", e.dictionary["second"]);
	}


	FACT("Complex types can be parsed")
	{
		auto e = decode<json2, ComplexEntity>(COMPLEX_JSON);

		Assert.Equal(3, e.collection.strings.size());
		Assert.Equal(2, e.entities.size());
		Assert.Equal("alternative", e.entities[0].name);
		Assert.Equal("complex", e.name);
		Assert.Equal(12345678, e.simple.bignumber);
	}


	FACT("Can cope with an empty object")
	{
		Assert.Equal("", decode<json2, TextEntity>("{}").text);
	}


	FACT("Can cope with alternative style line endings")
	{
		Assert.Equal("hello", decode<json2, TextEntity>("{ \"a\": 1,\r\n\"text\": \"hello\" }").text);
	}


	FACT("Can cope with standard JSON number formats")
	{
		Assert.Equal(42, decode<json2, SimpleEntity>(u8R"json({ "integer": 42 })json").integer);
		Assert.Equal(3.14, decode<json2, SimpleEntity>(u8R"json({ "floating": 3.14 })json").floating);
		Assert.Equal(3.141e-10, decode<json2, SimpleEntity>(u8R"json({ "floating": 3.141e-10 })json").floating);
		Assert.Equal(3.141e-10, decode<json2, SimpleEntity>(u8R"json({ "floating": 3.141E-10 })json").floating);
		Assert.Equal(12345123456789, decode<json2, SimpleEntity>(u8R"json({ "bignumber": 12345123456789 })json").bignumber);
		Assert.Equal(123456789123456789123456789.0, decode<json2, SimpleEntity>(u8R"json({ "floating": 123456789123456789123456789 })json").floating);
	}


	FACT("Will ignore unicode encodings")
	{
		Assert.Equal("\\u2000\\u20ff", decode<json2, TextEntity>(u8R"json({ "text":"\u2000\u20ff" })json").text);
	}


	FACT("Will support unprotected forward slashes")
	{
		Assert.Equal("http://something", decode<json2, TextEntity>(u8R"json({ "text":"http://something" })json").text);
	}


	vector<tuple<string>> invalid_json()
	{
		return {
			make_tuple(u8R"json({)json"),						// Missing top-level brace
			make_tuple(u8R"json({ "a": { "b": 42 })json"),		// Missing brace
			make_tuple(u8R"json({ "a: 42 })json"),				// Missing quotes in key
			make_tuple(u8R"json({ "a": "value })json"),			// Missing quotes in value
			make_tuple(u8R"json({ "a" "value" })json"),			// Missing key/value separator
			make_tuple(u8R"json({ "a": 1 b: 2 })json"),			// Missing item separator
			make_tuple(u8R"json({ "a": [ 1, 2, b: 42 })json"),	// Missing square bracket
			make_tuple(u8R"json({ "integer": invalid })json")	// Invalid value type
		};
	}


	DATA_THEORY("Will throw exception if the JSON is invalid", (string invalid), invalid_json)
	{
		Assert.Throws<exception>([&](){ decode<json2, SimpleEntity>(invalid); });
	}
}
