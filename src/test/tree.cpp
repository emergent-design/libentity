#include "catch.hpp"
#include <entity/tree.hpp>

using namespace std;
using namespace ent;


TEST_CASE("non-existent scalar properties return default values", "[tree]")
{
	REQUIRE(tree()["donotexist"].children.empty());
	REQUIRE(tree()["donotexist"].as_bool(true));
	REQUIRE(tree()["donotexist"].as_string("something") == "something");
	REQUIRE(tree()["donotexist"].as_long(42) == 42);
	REQUIRE(tree()["donotexist"].as_double(42.5) == 42.5);
}


TEST_CASE("non-existent array properties return empty containers", "[tree]")
{
	REQUIRE(tree()["donotexist"].as_array().empty());
}


TEST_CASE("setting a simple property permits it to be retrieved", "[tree]")
{
	REQUIRE(tree().set("simple", "something")["simple"].as_string() == "something");
	REQUIRE(tree().set("simple", 42)["simple"].as_long() == 42);
	REQUIRE(tree().set("simple", 42.2)["simple"].as_double() == 42.2);
	REQUIRE(tree().set("simple", true)["simple"].as_bool());
}


TEST_CASE("a tree representing an array can be accessed by index", "[tree]")
{
	REQUIRE(tree(vector<tree> { 1, 2, 3 })[1].as_long() == 2);
	REQUIRE(tree(vector<tree> { "a", "b", "c" })[2].as_string() == "c");
}


TEST_CASE("trees can be compared", "[tree]")
{
	REQUIRE(tree(42.0) == tree(42.0));
	REQUIRE(tree(42.0) == tree(42));
	REQUIRE(tree(42) != tree(13));
	REQUIRE(tree("hello") == tree("hello"));
	REQUIRE(tree("hello") != tree("world"));
	REQUIRE(tree() == tree());
	REQUIRE(tree().set("simple", 42) == tree().set("simple", 42));
	REQUIRE(tree().set("simple", 42) != tree().set("simple", 41));
	REQUIRE(tree().set("array", vector<tree> { 1, 2, 3 }) == tree().set("array", vector<tree> { 1, 2, 3 }));
	REQUIRE(tree().set("array", vector<tree> { 1, 2, 3 }) != tree().set("array", vector<tree> { 1, 2, 3.1 }));
}


TEST_CASE("a property can be another tree", "[tree]")
{
	REQUIRE(tree().set("tree", tree().set("child", 42))["tree"]["child"].as_long() == 42);
}


TEST_CASE("binary data can be set and retrieved as a vector<uint8_t>", "[tree]")
{
	REQUIRE(tree().set("binary", vector<uint8_t> { 0x00, 0x01, 0x02, 0x88, 0xff })["binary"].as_binary()[3] == 0x88);
}


TEST_CASE("a property can be an array of trees", "[tree]")
{
	tree t = {{
		"array", vector<tree> {
			{{ "name", "first" }},
			{{ "name", "second" }}
		}
	}};

	REQUIRE(t["array"].as_array()[1]["name"].as_string() == "second");
}


TEST_CASE("a complex tree can be set using a fluent style interface", "[tree]")
{
	auto t = tree()
		.set("string", "string value")
		.set("integer", 42)
		.set("floating", 3.14)
		.set("boolean", true)
		.set("array", vector<tree> { 1, 2, 3 })
		.set("tree", tree().set("name", "child"));

	REQUIRE(t["string"].as_string()				== "string value");
	REQUIRE(t["integer"].as_long()				== 42);
	REQUIRE(t["floating"].as_double()			== 3.14);
	REQUIRE(t["array"].as_array()[1].as_long()	== 2);
	REQUIRE(t["tree"]["name"].as_string()		== "child");
	REQUIRE(t["boolean"].as_bool());
}


TEST_CASE("a complex tree can be created using an initialisation list", "[tree]")
{
	tree t = {
		{ "string", "string value" },
		{ "integer", 42 },
		{ "floating", 3.14 },
		{ "boolean", true },
		{ "array", vector<tree> { 1, 2, 3 }},
		{ "tree", {
			{ "name", "child" }
		}}
	};

	REQUIRE(t["string"].as_string()				== "string value");
	REQUIRE(t["integer"].as_long()				== 42);
	REQUIRE(t["floating"].as_double()			== 3.14);
	REQUIRE(t["array"].as_array()[1].as_long()	== 2);
	REQUIRE(t["tree"]["name"].as_string()		== "child");
	REQUIRE(t["boolean"].as_bool());
}

