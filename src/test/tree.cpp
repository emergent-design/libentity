#include "doctest.h"
#include <entity/tree.hpp>

using namespace std;
using namespace ent;

TEST_SUITE("tree")
{
	TEST_CASE("non-existent scalar properties return default values")
	{
		CHECK(tree()["donotexist"].children.empty());
		CHECK(tree()["donotexist"].as_bool(true));
		CHECK(tree()["donotexist"].as_string("something") == "something");
		CHECK(tree()["donotexist"].as_long(42) == 42);
		CHECK(tree()["donotexist"].as_double(42.5) == 42.5);
	}


	TEST_CASE("non-existent array properties return empty containers")
	{
		CHECK(tree()["donotexist"].as_array().empty());
	}


	TEST_CASE("setting a simple property permits it to be retrieved")
	{
		CHECK(tree().set("simple", "something")["simple"].as_string() == "something");
		CHECK(tree().set("simple", 42)["simple"].as_long() == 42);
		CHECK(tree().set("simple", 42.2)["simple"].as_double() == 42.2);
		CHECK(tree().set("simple", true)["simple"].as_bool());
	}


	TEST_CASE("a tree representing an array can be accessed by index")
	{
		CHECK(tree(vector<tree> { 1, 2, 3 })[1].as_long() == 2);
		CHECK(tree(vector<tree> { "a", "b", "c" })[2].as_string() == "c");
	}


	TEST_CASE("trees can be compared")
	{
		CHECK(tree(42.0) == tree(42.0));
		CHECK(tree(42.0) == tree(42));
		CHECK(tree(42) != tree(13));
		CHECK(tree("hello") == tree("hello"));
		CHECK(tree("hello") != tree("world"));
		CHECK(tree() == tree());
		CHECK(tree().set("simple", 42) == tree().set("simple", 42));
		CHECK(tree().set("simple", 42) != tree().set("simple", 41));
		CHECK(tree().set("array", vector<tree> { 1, 2, 3 }) == tree().set("array", vector<tree> { 1, 2, 3 }));
		CHECK(tree().set("array", vector<tree> { 1, 2, 3 }) != tree().set("array", vector<tree> { 1, 2, 3.1 }));
	}


	TEST_CASE("a property can be another tree")
	{
		CHECK(tree().set("tree", tree().set("child", 42))["tree"]["child"].as_long() == 42);
	}


	TEST_CASE("binary data can be set and retrieved as a vector<uint8_t>)")
	{
		CHECK(tree().set("binary", vector<uint8_t> { 0x00, 0x01, 0x02, 0x88, 0xff })["binary"].as_binary()[3] == 0x88);
	}


	TEST_CASE("a property can be an array of trees")
	{
		tree t = {{
			"array", vector<tree> {
				{{ "name", "first" }},
				{{ "name", "second" }}
			}
		}};

		CHECK(t["array"].as_array()[1]["name"].as_string() == "second");
	}


	TEST_CASE("a complex tree can be set using a fluent style interface")
	{
		auto t = tree()
			.set("string", "string value")
			.set("integer", 42)
			.set("floating", 3.14)
			.set("boolean", true)
			.set("array", vector<tree> { 1, 2, 3 })
			.set("tree", tree().set("name", "child"));

		CHECK(t["string"].as_string()				== "string value");
		CHECK(t["integer"].as_long()				== 42);
		CHECK(t["floating"].as_double()				== 3.14);
		CHECK(t["array"].as_array()[1].as_long()	== 2);
		CHECK(t["tree"]["name"].as_string()			== "child");
		CHECK(t["boolean"].as_bool());
	}


	TEST_CASE("a complex tree can be created using an initialisation list")
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

		CHECK(t["string"].as_string()				== "string value");
		CHECK(t["integer"].as_long()				== 42);
		CHECK(t["floating"].as_double()				== 3.14);
		CHECK(t["array"].as_array()[1].as_long()	== 2);
		CHECK(t["tree"]["name"].as_string()			== "child");
		CHECK(t["boolean"].as_bool());
	}


	TEST_CASE("a property can be removed")
	{
		auto t = tree()
			.set("integer", 42)
			.set("floating", 3.14);

		CHECK(t.contains("integer"));

		t.erase("integer");

		CHECK(!t.contains("integer"));
		CHECK(t == tree()
			.set("floating", 3.14)
		);
	}
}
