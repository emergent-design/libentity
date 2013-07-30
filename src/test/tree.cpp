#include <xUnit++/xUnit++.h>
#include <entity/entity.h>

using namespace std;
using namespace ent;


SUITE("Tree Tests")
{
	FACT("Non-existent scalar properties return default values")
	{
		Assert.True(tree().get("donotexist").properties.empty());
		Assert.True(tree().get("donotexist", true));
		Assert.Equal("something",	tree().get("donotexist", string("something")));
		Assert.Equal(42,			tree().get("donotexist", 42));
		Assert.Equal(42.5,			tree().get("donotexist", 42.5));
	}


	FACT("Non-existent collection properties return empty containers")
	{
		Assert.True(tree().array<int>("donotexist").empty());
		Assert.True(tree().map<string>("donotexist").empty());
	}


	FACT("Setting a simple property permits it to be retrieved")
	{
		Assert.Equal("something",	tree().set("simple", "something").get<string>("simple"));
		Assert.Equal(42,			tree().set("simple", 42).get<int>("simple"));
		Assert.Equal(42.2,			tree().set("simple", 42.2).get<double>("simple"));
		Assert.True(tree().set("simple", true).get<bool>("simple"));	
	}


	FACT("A property can be set and retrieved as an array of simple items")
	{
		Assert.Equal(2, tree().set("array", vector<int> { 1, 2, 3, 4}).array<int>("array")[1]);
	}


	FACT("A property can be set and retrieved using a map<string, T>")
	{
		auto m = map<string, int> {{ "one", 1 }, { "two", 2 }};
		
		Assert.Equal(value::Type::Object,	tree().set("map", m).properties["map"].get_type());
		Assert.Equal(2,						tree().set("map", m).map<int>("map")["two"]);
	}


	FACT("A property can be another tree")
	{
		Assert.Equal(42, tree().set("tree", tree().set("child", 42)).get("tree").get<int>("child"));
	}


	FACT("Binary data can be set and retrieved as a vector<byte>")
	{
		Assert.Equal(0x88, tree().set("binary", vector<byte> { 0x00, 0x01, 0x02, 0x88, 0xff }).get<vector<byte>>("binary")[3]);
	}


	FACT("A property can be an array of trees")
	{
		auto t = tree().set("array", vector<tree> { tree().set("name", "first"), tree().set("name", "second") });

		Assert.Equal("second", t.array<tree>("array")[1].get<string>("name"));
	}


	FACT("A complex tree can be set using a fluent style interface")
	{
		auto t = tree()
			.set("string", "string value")
			.set("integer", 42)
			.set("floating", 3.14)
			.set("boolean", true)
			.set("array", vector<int> { 1, 2, 3 })
			.set("tree", tree().set("name", "child"));

		Assert.Equal("string value",	t.get<string>("string"));
		Assert.Equal(42,				t.get<int>("integer"));
		Assert.Equal(3.14,				t.get<double>("floating"));
		Assert.Equal(2,					t.array<int>("array")[1]);
		Assert.Equal("child",			t.get("tree").get<string>("name"));
		Assert.True(t.get<bool>("boolean"));
	}
}
