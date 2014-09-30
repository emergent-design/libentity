#include <xUnit++/xUnit++.h>
#include <entity/tree.hpp>

using namespace std;
using namespace ent;


SUITE("Tree Tests")
{
	FACT("Non-existent scalar properties return default values")
	{
		Assert.True(tree()["donotexist"].children.empty());
		Assert.True(tree()["donotexist"].as_bool(true));
		Assert.Equal("something",	tree()["donotexist"].as_string("something"));
		Assert.Equal(42,			tree()["donotexist"].as_long(42));
		Assert.Equal(42.5,			tree()["donotexist"].as_double(42.5));
	}


	FACT("Non-existent array properties return empty containers")
	{
		Assert.True(tree()["donotexist"].as_array().empty());
	}


	FACT("Setting a simple property permits it to be retrieved")
	{
		Assert.Equal("something",	tree().set("simple", "something")["simple"].as_string());
		Assert.Equal(42,			tree().set("simple", 42)["simple"].as_long());
		Assert.Equal(42.2,			tree().set("simple", 42.2)["simple"].as_double());
		Assert.True(tree().set("simple", true)["simple"].as_bool());
	}


	FACT("A property can be another tree")
	{
		Assert.Equal(42, tree().set("tree", tree().set("child", 42))["tree"]["child"].as_long());
	}


	FACT("Binary data can be set and retrieved as a vector<byte>")
	{
		Assert.Equal(0x88, tree().set("binary", vector<byte> { 0x00, 0x01, 0x02, 0x88, 0xff })["binary"].as_binary()[3]);
	}


	FACT("A property can be an array of trees")
	{
		tree t = {{
			"array", vector<tree> {
				{{ "name", "first" }},
				{{ "name", "second" }}
			}
		}};

		Assert.Equal("second", t["array"].as_array()[1]["name"].as_string());
	}


	FACT("A complex tree can be set using a fluent style interface")
	{
		auto t = tree()
			.set("string", "string value")
			.set("integer", 42)
			.set("floating", 3.14)
			.set("boolean", true)
			.set("array", vector<tree> { 1, 2, 3 })
			.set("tree", tree().set("name", "child"));

		Assert.Equal("string value",	t["string"].as_string());
		Assert.Equal(42,				t["integer"].as_long());
		Assert.Equal(3.14,				t["floating"].as_double());
		Assert.Equal(2,					t["array"].as_array()[1].as_long());
		Assert.Equal("child",			t["tree"]["name"].as_string());
		Assert.True(t["boolean"].as_bool());
	}
}
