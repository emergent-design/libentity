#include <xUnit++/xUnit++.h>
#include <entity/entity.hpp>
#include <entity/json.hpp>

using namespace std;
using namespace ent;


const string PADDED_JSON = u8R"json({
  "flag": true,
  "integer": 42,
  "name": "simple"
})json";


SUITE("JSON Tests")
{
	FACT("Compact JSON can be generated")
	{
		tree t = { {"name", "simple" }, { "integer", 42 }, { "flag", true} };

		Assert.Equal(u8R"json({"flag":true,"integer":42,"name":"simple"})json", tree::encode<json>(t));
	}


	FACT("Compact JSON can be parsed")
	{
		auto t = tree::decode<json>(u8R"json({"flag":true,"name":"simple","integer":42})json");

		Assert.Equal("simple", t["name"].as_string());
		Assert.Equal(42, t["integer"].as_long());
		Assert.True(t["flag"].as_bool());
	}


	FACT("Padded JSON can be generated")
	{
		tree t = { { "name", "simple" }, {"integer", 42}, {"flag", true} };

		Assert.Equal(PADDED_JSON, tree::encode<prettyjson>(t));
	}


	FACT("Padded JSON can be parsed")
	{
		auto t = tree::decode<json>(u8R"json({
			"flag": true,
			"integer": 42
			"name": "simple",
		})json");

		Assert.Equal("simple", t["name"].as_string());
		Assert.Equal(42, t["integer"].as_long());
		Assert.True(t["flag"].as_bool());
	}


	FACT("Strings are escaped appropriately")
	{
		tree t = { {"text", "Must\tbe \"escaped\"\n"} };

		Assert.Equal(u8R"json({"text":"Must\tbe \"escaped\"\n"})json", tree::encode<json>(t));
	}


	FACT("Strings are unescaped appropriately")
	{
		auto t = tree::decode<json>(u8R"json({ "text": "Must\tbe \"escaped\"\n" })json");

		Assert.Equal("Must\tbe \"escaped\"\n", t["text"].as_string());
	}


	FACT("Simple types can be parsed")
	{
		auto t = tree::decode<json>(u8R"json({
			"name": "simple",
			"integer": 42,
			"double": 3.14,
			"flag": true,
			"nothing": null,
		})json");

		Assert.Equal(tree::Type::String,	t["name"].get_type());
		Assert.Equal(tree::Type::Integer,	t["integer"].get_type());
		Assert.Equal(tree::Type::Floating,	t["double"].get_type());
		Assert.Equal(tree::Type::Boolean,	t["flag"].get_type());
		Assert.Equal(tree::Type::Null,		t["nothing"].get_type());
	}


	FACT("Arrays can be parsed")
	{
		auto t = tree::decode<json>(u8R"json({
			"array": [ 1, 2, 3, 4 ]
		})json");

		Assert.Equal(tree::Type::Array,	t["array"].get_type());
		Assert.Equal(4,					t["array"].as_array().size());
	}


	FACT("Compact arrays can be parsed")
	{
		auto t = tree::decode<json>(u8R"json({
			"array": [1,2,3,4]
		})json");

		Assert.Equal(tree::Type::Array,	t["array"].get_type());
		Assert.Equal(4,					t["array"].as_array().size());
	}


	FACT("Object trees can be parsed")
	{
		auto t = tree::decode<json>(u8R"json({
			"object": {
				"name": "complex"
			}
		})json");

		Assert.Equal(tree::Type::Object,	t["object"].get_type());
		Assert.Equal("complex",				t["object"]["name"].as_string());
	}


	FACT("Can cope with an empty object")
	{
		Assert.Equal(0, tree::decode<json>("{}").children.size());
	}


	FACT("Can cope with alternative style line endings")
	{
		Assert.Equal(2, tree::decode<json>("{ \"a\": 1,\r\n\"b\": 2 }")["b"].as_long());
	}


	FACT("Can cope with standard JSON number formats")
	{
		// Largest exact integral size represented by JSON is 2^53 so anything bigger
		// (like "big") must be handled as a double and therefore accuracy will be lost.

		auto t = tree::decode<json>(u8R"json({
			"integer": 42,
			"double": 3.14,
			"scientific": 3.141e-10,
			"upper": 3.141E-10,
			"long": 12345123456789,
			"big": 123456789123456789123456789.0
		})json");

		Assert.Equal(42,							t["integer"].as_long());
		Assert.Equal(3.14,							t["double"].as_double());
		Assert.Equal(3.141e-10,						t["scientific"].as_double());
		Assert.Equal(3.141e-10,						t["upper"].as_double());
		Assert.Equal(12345123456789,				t["long"].as_long());
		Assert.Equal(123456789123456789123456789.0,	t["big"].as_double());
	}


	FACT("Will ignore unicode encodings")
	{
		Assert.Equal("\\u2000\\u20ff", tree::decode<json>(u8R"json({ "text":"\u2000\u20ff" })json")["text"].as_string());
	}

	FACT("Supports arrays of objects")
	{
		auto t = tree::decode<json>(u8R"json({
			"coords":[{"x":0,"y":0},{"x":1,"y":1},{"x":2,"y":2}]
		})json");

		Assert.Equal(tree::Type::Array,	t["coords"].get_type());
		Assert.Equal(0, t["coords"].as_array()[0]["x"].as_long());
		Assert.Equal(0, t["coords"].as_array()[0]["y"].as_long());
		Assert.Equal(1, t["coords"].as_array()[1]["x"].as_long());
		Assert.Equal(1, t["coords"].as_array()[1]["y"].as_long());
		Assert.Equal(2, t["coords"].as_array()[2]["x"].as_long());
		Assert.Equal(2, t["coords"].as_array()[2]["y"].as_long());
	}


	FACT("Will support unprotected forward slashes")
	{
		Assert.Equal("http://something", tree::decode<json>(u8R"json({ "text":"http://something" })json")["text"].as_string());
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
			make_tuple(u8R"json({ "a": invalid })json")			// Invalid value type
		};
	}


	DATA_THEORY("Will throw exception if the JSON is invalid", (string invalid), invalid_json)
	{
		Assert.Throws<exception>([&](){ tree::decode<json>(invalid); });
	}
}


