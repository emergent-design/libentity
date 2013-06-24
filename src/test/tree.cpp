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
		Assert.Equal(tree().get("donotexist", string("something")), "something");
		Assert.Equal(tree().get("donotexist", 42), 42);
		Assert.Equal(tree().get("donotexist", 42.5), 42.5);
	}


	FACT("Non-existent collection properties return empty containers")
	{
		Assert.True(tree().array<int>("donotexist").empty());
		Assert.True(tree().map<string>("donotexist").empty());
	}


	FACT("Setting a simple property permits it to be retrieved")
	{
		Assert.Equal(tree().set("simple", "something").get<string>("simple"), "something");
		Assert.Equal(tree().set("simple", 42).get<int>("simple"), 42);
		Assert.Equal(tree().set("simple", 42.2).get<double>("simple"), 42.2);
		Assert.True(tree().set("simple", true).get<bool>("simple"));	
	}
}
