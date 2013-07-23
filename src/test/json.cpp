#include <xUnit++/xUnit++.h>
#include <entity/entity.h>
#include <entity/json.h>

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
		auto t = tree().set("name", "simple").set("integer", 42).set("flag", true);

		Assert.Equal(u8R"json({"flag":true,"integer":42,"name":"simple"})json", json::to(t, false));
	}


	FACT("Compact JSON can be parsed")
	{
		auto t = json::from(u8R"json({"flag":true,"name":"simple","integer":42})json");

		Assert.Equal("simple", t.get<string>("name"));
		Assert.Equal(42, t.get<int>("integer"));
		Assert.True(t.get<bool>("flag"));
	}


	FACT("Padded JSON can be generated")
	{
		auto t = tree().set("name", "simple").set("integer", 42).set("flag", true);

		Assert.Equal(PADDED_JSON, json::to(t, true));
	}


	FACT("Padded JSON can be parsed")
	{
		auto t = json::from(u8R"json({
			"flag": true,
			"integer": 42
			"name": "simple",
		})json");

		Assert.Equal("simple", t.get<string>("name"));
		Assert.Equal(42, t.get<int>("integer"));
		Assert.True(t.get<bool>("flag"));
	}


	FACT("Strings are escaped appropriately")
	{
		auto t = tree().set("text", "Must\tbe \"escaped\"\n");

		Assert.Equal(u8R"json({"text":"Must\tbe \"escaped\"\n"})json", json::to(t, false));
	}


	FACT("Strings are unescaped appropriately")
	{
		auto t = json::from(u8R"json({ "text": "Must\tbe \"escaped\"\n" })json");

		Assert.Equal("Must\tbe \"escaped\"\n", t.get<string>("text"));
	}

	
	FACT("Simple types can be parsed")
	{
		auto t = json::from(u8R"json({
			"name": "simple",
			"integer": 42,
			"double": 3.14,
			"flag": true,
			"nothing": null,
		})json");

		Assert.Equal(vtype::String,		t.properties["name"].type);
		Assert.Equal(vtype::Number,		t.properties["integer"].type);
		Assert.Equal(vtype::Number,		t.properties["double"].type);
		Assert.Equal(vtype::Boolean,	t.properties["flag"].type);
		Assert.Equal(vtype::Null,		t.properties["nothing"].type);
	}


	FACT("Arrays can be parsed")
	{
		auto t = json::from(u8R"json({
			"array": [ 1, 2, 3, 4 ]
		})json");

		Assert.Equal(vtype::Array,	t.properties["array"].type);
		Assert.Equal(4,				t.properties["array"].array.size());
	}


	FACT("Object trees can be parsed")
	{
		auto t = json::from(u8R"json({
			"object": {
				"name": "complex"
			}
		})json");

		Assert.Equal(vtype::Object,	t.properties["object"].type);
		Assert.Equal("complex",		t.properties["object"].object->get<string>("name"));
	}


	FACT("Can cope with an empty object")
	{
		Assert.Equal(0, json::from("{}").properties.size());
	}


	FACT("Can cope with alternative style line endings")
	{
		Assert.Equal(2, json::from("{ \"a\": 1,\r\n\"b\": 2 }").get<int>("b"));
	}


	FACT("Can cope with standard JSON number formats")
	{
		auto t = json::from(u8R"json({
			"integer": 42,
			"double": 3.14,
			"scientific": 3.141e-10,
			"upper": 3.141E-10,
			"long": 12345123456789,
			"big": 123456789123456789123456789
		})json");

		Assert.Equal(42,							t.get<int>("integer"));
		Assert.Equal(3.14,							t.get<double>("double"));
		Assert.Equal(3.141e-10,						t.get<double>("scientific"));
		Assert.Equal(3.141e-10,						t.get<double>("upper"));
		Assert.Equal(12345123456789,				t.get<long>("long"));
		Assert.Equal(123456789123456789123456789.0,	t.get<double>("big"));
	}


	FACT("Will ignore unicode encodings")
	{
		Assert.Equal("\\u2000\\u20ff", json::from(u8R"json({ "text":"\u2000\u20ff" })json").get<string>("text"));
	}


	FACT("Will support unprotected forward slashes")
	{
		Assert.Equal("http://something", json::from(u8R"json({ "text":"http://something" })json").get<string>("text"));
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
		Assert.Throws<exception>([&](){ json::from(invalid); });
	}
}


