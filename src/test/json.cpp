#include "doctest.h"
#include <entity/entity.hpp>
#include <entity/json.hpp>

using namespace std;
using namespace ent;

const string PADDED_JSON = R"json({
  "flag": true,
  "integer": 42,
  "name": "simple"
})json";


TEST_SUITE("json")
{
	TEST_CASE("compact JSON can be generated")
	{
		tree t = { {"name", "simple" }, { "integer", 42 }, { "flag", true} };

		CHECK(encode<json>(t) == R"json({"flag":true,"integer":42,"name":"simple"})json");
	}


	TEST_CASE("compact JSON can be parsed")
	{
		auto t = decode<json>(R"json({"flag":true,"name":"simple","integer":42})json");

		CHECK(t["name"].as_string() == "simple");
		CHECK(t["integer"].as_long() == 42);
		CHECK(t["flag"].as_bool());
	}


	TEST_CASE("padded JSON can be generated")
	{
		tree t = { { "name", "simple" }, {"integer", 42}, {"flag", true} };

		CHECK(encode<prettyjson>(t) == PADDED_JSON);
	}


	TEST_CASE("padded JSON can be parsed")
	{
		auto t = decode<json>(R"json({
			"flag": true,
			"integer": 42,
			"name": "simple",
		})json");

		CHECK(t["name"].as_string() == "simple");
		CHECK(t["integer"].as_long() == 42);
		CHECK(t["flag"].as_bool());
	}


	TEST_CASE("strings are escaped appropriately")
	{
		tree t = { {"text", "Must\tbe \"escaped\"\n"} };

		CHECK(encode<json>(t) == R"json({"text":"Must\tbe \"escaped\"\n"})json");
	}


	TEST_CASE("strings are unescaped appropriately")
	{
		auto t = decode<json>(R"json({ "text": "Must\tbe \"escaped\"\n" })json");

		CHECK(t["text"].as_string() == "Must\tbe \"escaped\"\n");
	}


	TEST_CASE("simple types can be parsed")
	{
		auto t = decode<json>(R"json({
			"name": "simple",
			"integer": 42,
			"double": 3.14,
			"flag": true,
			"nothing": null,
		})json");

		CHECK(t["name"].get_type()	== tree::Type::String);
		CHECK(t["integer"].get_type()	== tree::Type::Integer);
		CHECK(t["double"].get_type()	== tree::Type::Floating);
		CHECK(t["flag"].get_type()	== tree::Type::Boolean);
		CHECK(t["nothing"].get_type()	== tree::Type::Null);
	}


	TEST_CASE("arrays can be parsed")
	{
		auto t = decode<json>(R"json({
			"array": [ 1, 2, 3, 4 ]
		})json");

		CHECK(t["array"].get_type() == tree::Type::Array);
		CHECK(t["array"].as_array().size() == 4);
	}


	TEST_CASE("compact arrays can be parsed")
	{
		auto t = decode<json>(R"json({
			"array": [1,2,3,4]
		})json");

		CHECK(t["array"].get_type() == tree::Type::Array);
		CHECK(t["array"].as_array().size() == 4);
	}


	TEST_CASE("top level arrays can be generated")
	{
		tree t = vector<tree> { 1, 2, 3, 4 };

		CHECK(encode<json>(t) == "[1,2,3,4]");
	}


	TEST_CASE("top level arrays can be parsed")
	{
		auto t = decode<json>("[1,2,3,4]");

		CHECK(t.get_type() == tree::Type::Array);
		CHECK(t.as_array().size() == 4);
	}


	TEST_CASE("object trees can be parsed")
	{
		auto t = decode<json>(R"json({
			"object": {
				"name": "complex"
			}
		})json");

		CHECK(t["object"].get_type() == tree::Type::Object);
		CHECK(t["object"]["name"].as_string() == "complex");
	}


	TEST_CASE("can cope with an empty object")
	{
		CHECK(decode<json>("{}").children.size() == 0);
	}


	TEST_CASE("can cope with alternative style) // line endings")
	{
		CHECK(decode<json>("{ \"a\": 1,\r\n\"b\": 2 }")["b"].as_long() == 2);
	}


	TEST_CASE("can cope with standard JSON number formats")
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

		CHECK(t["integer"].as_long()		== 42);
		CHECK(t["double"].as_double()		== 3.14);
		CHECK(t["scientific"].as_double()	== 3.141e-10);
		CHECK(t["upper"].as_double()		== 3.141e-10);
		CHECK(t["long"].as_long()			== 12345123456789);
		CHECK(t["big"].as_double()		== 123456789123456789123456789.0);
		CHECK(t["tiny"].as_double()		== 1.0e-300);
	}


	TEST_CASE("converts NaN and infite values to null")
	{
		tree t = {
			{ "infinity", INFINITY },
			{ "nan", NAN }
		};

		CHECK(encode<json>(t) == R"json({"infinity":null,"nan":null})json");
	}


	TEST_CASE("will ignore unicode encodings")
	{
		CHECK(decode<json>(R"json({ "text":"\u2000\u20ff" })json")["text"].as_string() == "\\u2000\\u20ff");
	}

	TEST_CASE("supports arrays of objects")
	{
		auto t = decode<json>(R"json({
			"coords":[{"x":0,"y":0},{"x":1,"y":1},{"x":2,"y":2}]
		})json");

		CHECK(t["coords"].get_type()						== tree::Type::Array);
		CHECK(t["coords"].as_array()[0]["x"].as_long()	== 0);
		CHECK(t["coords"].as_array()[0]["y"].as_long()	== 0);
		CHECK(t["coords"].as_array()[1]["x"].as_long()	== 1);
		CHECK(t["coords"].as_array()[1]["y"].as_long()	== 1);
		CHECK(t["coords"].as_array()[2]["x"].as_long()	== 2);
		CHECK(t["coords"].as_array()[2]["y"].as_long()	== 2);
	}


	TEST_CASE("will support unprotected forward slashes")
	{
		string JSON = R"json({ "text":"http://something" })json";

		CHECK(decode<json>(JSON)["text"].as_string() == "http://something");
	}


	TEST_CASE("parser will throw exception if the JSON is invalid")
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
			CHECK_THROWS(decode<json>(i));
		}
	}


	//---------------- JSON5 extended syntax

	TEST_CASE("parser supports single line comments")
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


		CHECK(t["name"].get_type()	== tree::Type::String);
		CHECK(t["integer"].get_type()	== tree::Type::Integer);
		CHECK(t["double"].get_type()	== tree::Type::Floating);
		CHECK(t["nothing"].get_type()	== tree::Type::Null);
		CHECK(!t.contains("flag"));
	}


	TEST_CASE("parser supports block comments")
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

		CHECK(t["name"].get_type()	== tree::Type::String);
		CHECK(t["nothing"].get_type()	== tree::Type::Null);
		CHECK(!t.contains("integer"));
		CHECK(!t.contains("double"));
		CHECK(!t.contains("flag"));
	}


	TEST_CASE("parser supports trailing commas")
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

		CHECK(t["object"].get_type() == tree::Type::Object);
		CHECK(t["object"]["name"].as_string() == "complex");
		CHECK(t["array"].get_type() == tree::Type::Array);
		CHECK(t["array"].as_array().size() == 4);
	}


	TEST_CASE("parser supports single quoted string")
	{

	}

	// TEST_CASE("parser supports multiline strings")
	// {

	// }

	// Strings may include character escapes?

	TEST_CASE("parser supports hexadecimal numbers")
	{
		auto t = decode<json>(R"json({
			"positive": 0xff,
			"negative": -0xff
		})json");

		CHECK(t["positive"].as_long() == 255);
		CHECK(t["negative"].as_long() == -255);
	}


	TEST_CASE("parser supports leading or trailing decimal points in numbers")
	{
		auto t = decode<json>(R"json({
			"leading": .25,
			"trailing": 42.
		})json");

		CHECK(t["leading"].as_double() == 0.25);
		CHECK(t["trailing"].as_double() == 42.0);
	}

	TEST_CASE("parser supports IEEE 754 positive/negative infinity) // and NaN")
	{
		// auto t = decode<json>(R"json({
		// 	"positive": Infinity,
		// 	"negative": -Infinity,
		// 	"nan": NaN
		// })json");

		// CHECK(t["positive"].as_double() == INFINITY);
		// CHECK(t["negative"].as_double() == -INFINITY);
		// CHECK(std::isnan(t["nan"].as_double()));

		// CHECK(encode<json>(t) == "");
	}

	TEST_CASE("parser supports numbers starting with a plus sign")
	{

	}

	TEST_CASE("parser ignores all forms of whitespace")
	{

	}
}
