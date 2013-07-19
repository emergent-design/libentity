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
		Assert.Equal("test", value("test").get((string)""));
	}


	FACT("Construction with different number types is a number")
	{
		Assert.Equal(vtype::Number,		value(42).type);
		Assert.Equal(vtype::Number,		value(42l).type);
		Assert.Equal(vtype::Number,		value(42.0f).type);
		Assert.Equal(vtype::Number,		value(42.0).type);
		Assert.NotEqual(vtype::Number,	value("test").type);
	}


	FACT("Construction with value vector is an array")
	{
		vector<value> values = { value(), value() };
		Assert.Equal(vtype::Array, value(values).type);
	}


	FACT("Construction with entity reference is an object")
	{
		Assert.Equal(vtype::Object, value(make_shared<tree>()).type);
	}


	FACT("Number value can be read as any supported numeric type")
	{
		Assert.Equal(42.0, value(42).get(0.0));
		Assert.Equal(42, value(42.1).get(0));
		Assert.Equal(42.1f, value(42.1).get(0.0f));
	}


	FACT("Boolean value is boolean type")
	{
		Assert.Equal(vtype::Boolean, value(true).type);
		Assert.True(value(true).get(false));
	}


	FACT("Reading as incorrect type returns the default value")
	{
		Assert.Equal(42, value("test").get(42));
	}


	FACT("Value can be copied")
	{
		value a(42);
		value b = a;

		Assert.Equal(42, a.get(0));
		Assert.Equal(vtype::Number, b.type);
		Assert.Equal(42, b.get(0));
	}
}

