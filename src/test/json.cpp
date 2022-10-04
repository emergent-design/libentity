#include "catch.hpp"
#include <entity/entity.hpp>
#include <entity/json.hpp>

using namespace std;
using namespace ent;


const string PADDED_JSON = R"json({
  "flag": true,
  "integer": 42,
  "name": "simple"
})json";


TEST_CASE("compact JSON can be generated", "[json]")
{
	tree t = { {"name", "simple" }, { "integer", 42 }, { "flag", true} };

	REQUIRE(encode<json>(t) == R"json({"flag":true,"integer":42,"name":"simple"})json");
}


TEST_CASE("compact JSON can be parsed", "[json]")
{
	auto t = decode<json>(R"json({"flag":true,"name":"simple","integer":42})json");

	REQUIRE(t["name"].as_string() == "simple");
	REQUIRE(t["integer"].as_long() == 42);
	REQUIRE(t["flag"].as_bool());
}


TEST_CASE("padded JSON can be generated", "[json]")
{
	tree t = { { "name", "simple" }, {"integer", 42}, {"flag", true} };

	REQUIRE(encode<prettyjson>(t) == PADDED_JSON);
}


TEST_CASE("padded JSON can be parsed", "[json]")
{
	auto t = decode<json>(R"json({
		"flag": true,
		"integer": 42,
		"name": "simple",
	})json");

	REQUIRE(t["name"].as_string() == "simple");
	REQUIRE(t["integer"].as_long() == 42);
	REQUIRE(t["flag"].as_bool());
}


TEST_CASE("strings are escaped appropriately", "[json]")
{
	tree t = { {"text", "Must\tbe \"escaped\"\n"} };

	REQUIRE(encode<json>(t) == R"json({"text":"Must\tbe \"escaped\"\n"})json");
}


TEST_CASE("strings are unescaped appropriately", "[json]")
{
	auto t = decode<json>(R"json({ "text": "Must\tbe \"escaped\"\n" })json");

	REQUIRE(t["text"].as_string() == "Must\tbe \"escaped\"\n");
}


TEST_CASE("simple types can be parsed", "[json]")
{
	auto t = decode<json>(R"json({
		"name": "simple",
		"integer": 42,
		"double": 3.14,
		"flag": true,
		"nothing": null,
	})json");

	REQUIRE(t["name"].get_type()	== tree::Type::String);
	REQUIRE(t["integer"].get_type()	== tree::Type::Integer);
	REQUIRE(t["double"].get_type()	== tree::Type::Floating);
	REQUIRE(t["flag"].get_type()	== tree::Type::Boolean);
	REQUIRE(t["nothing"].get_type()	== tree::Type::Null);
}


TEST_CASE("arrays can be parsed", "[json]")
{
	auto t = decode<json>(R"json({
		"array": [ 1, 2, 3, 4 ]
	})json");

	REQUIRE(t["array"].get_type() == tree::Type::Array);
	REQUIRE(t["array"].as_array().size() == 4);
}


TEST_CASE("compact arrays can be parsed", "[json]")
{
	auto t = decode<json>(R"json({
		"array": [1,2,3,4]
	})json");

	REQUIRE(t["array"].get_type() == tree::Type::Array);
	REQUIRE(t["array"].as_array().size() == 4);
}


TEST_CASE("top level arrays can be generated", "[json]")
{
	tree t = vector<tree> { 1, 2, 3, 4 };

	REQUIRE(encode<json>(t) == "[1,2,3,4]");
}


TEST_CASE("top level arrays can be parsed", "[json]")
{
	auto t = decode<json>("[1,2,3,4]");

	REQUIRE(t.get_type() == tree::Type::Array);
	REQUIRE(t.as_array().size() == 4);
}


TEST_CASE("object trees can be parsed", "[json]")
{
	auto t = decode<json>(R"json({
		"object": {
			"name": "complex"
		}
	})json");

	REQUIRE(t["object"].get_type() == tree::Type::Object);
	REQUIRE(t["object"]["name"].as_string() == "complex");
}


TEST_CASE("can cope with an empty object", "[json]")
{
	REQUIRE(decode<json>("{}").children.size() == 0);
}


TEST_CASE("can cope with alternative style line endings")
{
	REQUIRE(decode<json>("{ \"a\": 1,\r\n\"b\": 2 }")["b"].as_long() == 2);
}


TEST_CASE("can cope with standard JSON number formats", "[json]")
{
	// Largest exact integral size represented by JSON is 2^53 so anything bigger
	// (like "big") must be handled as a double and therefore accuracy will be lost.
	auto t = decode<json>(R"json({
		"integer": 42,
		"double": 3.14,
		"scientific": 3.141e-10,
		"upper": 3.141E-10,
		"long": 12345123456789,
		"big": 123456789123456789123456789.0,
		"tiny": 1.0e-300
	})json");

	REQUIRE(t["integer"].as_long()		== 42);
	REQUIRE(t["double"].as_double()		== 3.14);
	REQUIRE(t["scientific"].as_double()	== 3.141e-10);
	REQUIRE(t["upper"].as_double()		== 3.141e-10);
	REQUIRE(t["long"].as_long()			== 12345123456789);
	REQUIRE(t["big"].as_double()		== 123456789123456789123456789.0);
	REQUIRE(t["tiny"].as_double()		== 1.0e-300);
}


TEST_CASE("converts NaN and infite values to null", "[json]")
{
	tree t = {
		{ "infinity", INFINITY },
		{ "nan", NAN }
	};

	REQUIRE(encode<json>(t) == R"json({"infinity":null,"nan":null})json");
}


TEST_CASE("will ignore unicode encodings", "[json]")
{
	REQUIRE(decode<json>(R"json({ "text":"\u2000\u20ff" })json")["text"].as_string() == "\\u2000\\u20ff");
}

TEST_CASE("supports arrays of objects", "[json]")
{
	auto t = decode<json>(R"json({
		"coords":[{"x":0,"y":0},{"x":1,"y":1},{"x":2,"y":2}]
	})json");

	REQUIRE(t["coords"].get_type()						== tree::Type::Array);
	REQUIRE(t["coords"].as_array()[0]["x"].as_long()	== 0);
	REQUIRE(t["coords"].as_array()[0]["y"].as_long()	== 0);
	REQUIRE(t["coords"].as_array()[1]["x"].as_long()	== 1);
	REQUIRE(t["coords"].as_array()[1]["y"].as_long()	== 1);
	REQUIRE(t["coords"].as_array()[2]["x"].as_long()	== 2);
	REQUIRE(t["coords"].as_array()[2]["y"].as_long()	== 2);
}


TEST_CASE("will support unprotected forward slashes", "[json]")
{
	string JSON = R"json({ "text":"http://something" })json";

	REQUIRE(decode<json>(JSON)["text"].as_string() == "http://something");
}


TEST_CASE("parser will throw exception if the JSON is invalid", "[json]")
{
	vector<string> invalid_vectors = {
		R"json({)json",						// Missing top-level brace
		R"json({ "a": { "b": 42 })json",		// Missing brace
		R"json({ "a: 42 })json",				// Missing quotes in key
		R"json({ "a": "value })json",			// Missing quotes in value
		R"json({ "a" "value" })json",			// Missing key/value separator
		R"json({ "a": 1 b: 2 })json",			// Missing item separator
		R"json({ "a": [ 1, 2, b: 42 })json",	// Missing square bracket
		R"json({ "a": invalid })json"			// Invalid value type
	};

	for (auto &i : invalid_vectors)
	{
		REQUIRE_THROWS(decode<json>(i));
	}
}


//---------------- JSON5 extended syntax

TEST_CASE("parser supports single line comments", "[json]")
{
	auto t = decode<json>(R"json(
		{
			"name": "simple",
			"integer": 42,		// end of line comment
			"double": 3.14,
			// "flag": true,
			"nothing"//: something
				: null// adjacent
		}
	)json");


	REQUIRE(t["name"].get_type()	== tree::Type::String);
	REQUIRE(t["integer"].get_type()	== tree::Type::Integer);
	REQUIRE(t["double"].get_type()	== tree::Type::Floating);
	REQUIRE(t["nothing"].get_type()	== tree::Type::Null);
	REQUIRE(!t.contains("flag"));
}


TEST_CASE("parser supports block comments", "[json]")
{
	auto t = decode<json>(R"json(
		{
			"name": "simple",
			/* "integer": 42,
			"double": {,
			"flag": true, */
			"nothing": /*something*/null
		}
	)json");

	REQUIRE(t["name"].get_type()	== tree::Type::String);
	REQUIRE(t["nothing"].get_type()	== tree::Type::Null);
	REQUIRE(!t.contains("integer"));
	REQUIRE(!t.contains("double"));
	REQUIRE(!t.contains("flag"));
}


TEST_CASE("parser supports trailing commas", "[json]")
{
	// It actually appears that the decoder doesn't care whether or not
	// there are any commas since it treats commas as just another form
	// of whitespace.
	auto t = decode<json>(R"json({
		"object": {
			"name": "complex",
		},
		"array": [ 1, 2, 3, 4, ]
	})json");

	REQUIRE(t["object"].get_type() == tree::Type::Object);
	REQUIRE(t["object"]["name"].as_string() == "complex");
	REQUIRE(t["array"].get_type() == tree::Type::Array);
	REQUIRE(t["array"].as_array().size() == 4);
}


TEST_CASE("parser supports single quoted string", "[json]")
{

}

// TEST_CASE("parser supports multiline strings", "[json]")
// {

// }

// Strings may include character escapes?

TEST_CASE("parser supports hexadecimal numbers", "[json]")
{
	auto t = decode<json>(R"json({
		"positive": 0xff,
		"negative": -0xff
	})json");

	REQUIRE(t["positive"].as_long() == 255);
	REQUIRE(t["negative"].as_long() == -255);
}


TEST_CASE("parser supports leading or trailing decimal points in numbers", "[json]")
{
	auto t = decode<json>(R"json({
		"leading": .25,
		"trailing": 42.
	})json");

	REQUIRE(t["leading"].as_double() == 0.25);
	REQUIRE(t["trailing"].as_double() == 42.0);
}

TEST_CASE("parser supports IEEE 754 positive/negative infinity and NaN")
{
	// auto t = decode<json>(R"json({
	// 	"positive": Infinity,
	// 	"negative": -Infinity,
	// 	"nan": NaN
	// })json");

	// REQUIRE(t["positive"].as_double() == INFINITY);
	// REQUIRE(t["negative"].as_double() == -INFINITY);
	// REQUIRE(std::isnan(t["nan"].as_double()));

	// REQUIRE(encode<json>(t) == "");
}

TEST_CASE("parser supports numbers starting with a plus sign", "[json]")
{

}

TEST_CASE("parser ignores all forms of whitespace", "[json]")
{

}
