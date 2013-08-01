#include <xUnit++/xUnit++.h>
#include <entity/entity.h>

using namespace std;
using namespace ent;


SUITE("Value Tests")
{
	FACT("Construction with string literal is a string")
	{
		Assert.True(value("test").is<string>());
		Assert.Equal("test", value("test").get((string)""));
	}


	FACT("Construction with different number types is a number")
	{
		Assert.Equal(value::Type::Number,		value(42).get_type());
		Assert.Equal(value::Type::Number,		value(42l).get_type());
		Assert.Equal(value::Type::Number,		value(42.0f).get_type());
		Assert.Equal(value::Type::Number,		value(42.0).get_type());
		Assert.NotEqual(value::Type::Number,	value("test").get_type());
	}


	FACT("Construction with different number types kept as integral or floating point accordingly")
	{
		Assert.Equal(value::Number::Integer,	value(42).get_numtype());
		Assert.Equal(value::Number::Integer,	value(byte(42)).get_numtype());
		Assert.Equal(value::Number::Floating,	value(42.0).get_numtype());
		Assert.Equal(value::Number::Floating,	value(42.0f).get_numtype());
	}


	FACT("Construction with binary blob is supported")
	{
		Assert.Equal(value::Type::Binary, value(vector<byte> { 0x00, 0x01, 0x02 }).get_type());
	}


	FACT("Construction with value vector is an array")
	{
		vector<value> values = { value(), value() };
		Assert.Equal(value::Type::Array, value(values).get_type());
	}


	FACT("Construction with entity reference is an object")
	{
		Assert.Equal(value::Type::Object, value(make_shared<tree>()).get_type());
	}


	FACT("Number value can be read as any supported numeric type")
	{
		Assert.Equal(42.0, value(42).get(0.0));
		Assert.Equal(42, value(42.1).get(0));
		Assert.Equal(42.1f, value(42.1).get(0.0f));
	}


	FACT("Boolean value is boolean type")
	{
		Assert.Equal(value::Type::Boolean, value(true).get_type());
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
		Assert.Equal(value::Type::Number, b.get_type());
		Assert.Equal(42, b.get(0));
	}


	vector<tuple<value, value, value, value>> value_comparison_vectors()
	{
		return {
			make_tuple(value(42),							value(42),							value(10),							value("test")),	// Integer
			make_tuple(value(3.14),							value(3.14),						value(1.01),						value(42)),		// Floating-point
			make_tuple(value("test"),						value("test"),						value("wrong"),						value(3.4)),	// String
			make_tuple(value(true),							value(true),						value(false),						value("test")),	// Boolean
			make_tuple(value(), 							value(),							value("wrong"),						value(3.14)),	// Null
			make_tuple(value(vector<byte> { 0x00, 0xff }),	value(vector<byte> { 0x00, 0xff }), value(vector<byte> { 0x00, 0xaa }), value("test")),	// Binary
			make_tuple(value(vector<value> { value(42) }),	value(vector<value> { value(42) }), value(vector<value> { value(10) }), value("test")),	// Array
		};
	}


	DATA_THEORY("Simple values can be compared", (value data, value same, value wrong, value different), value_comparison_vectors)
	{
		Assert.True(data == same);
		Assert.False(data == wrong);
		Assert.False(data == different);
	}
}

