#include <xUnit++/xUnit++.h>
#include <entity/entity.h>

using namespace std;
using namespace ent;


SUITE("Value Tests")
{
	FACT("Default value is null")
	{
		Assert.True(value().null());
	}


	FACT("Construction with string literal is a string")
	{
		Assert.True(value("test").is<string>());
		Assert.Equal(value("test").get((string)""), (string)"test");
	}


	FACT("Construction with different number types is a number")
	{
		Assert.Equal(value(42).type, vtype::Number);
		Assert.Equal(value(42l).type, vtype::Number);
		Assert.Equal(value(42.0f).type, vtype::Number);
		Assert.Equal(value(42.0).type, vtype::Number);
		Assert.NotEqual(value("test").type, vtype::Number);
	}


	FACT("Construction with value vector is an array")
	{
		vector<value> values = { value(), value() };
		Assert.Equal(value(values).type, vtype::Array);
	}


	FACT("Construction with entity reference is an object")
	{
		Assert.Equal(value(make_shared<entity>()).type, vtype::Object);
	}


	FACT("Number value can be read as any supported numeric type")
	{
		Assert.Equal(value(42).get(0.0), 42.0);
		Assert.Equal(value(42.1).get(0), 42);
		Assert.Equal(value(42.1).get(0.0f), 42.1f);
	}


	FACT("Boolean value is boolean type")
	{
		Assert.Equal(value(true).type, vtype::Boolean);
		Assert.True(value(true).get(false));
	}


	FACT("Reading as incorrect type returns the default value")
	{
		Assert.Equal(value("test").get(42), 42);
	}


	FACT("Value can be copied")
	{
		value a(42);
		value b = a;

		Assert.Equal(a.get(0), 42);
		Assert.Equal(b.type, vtype::Number);
		Assert.Equal(b.get(0), 42);
	}
}

