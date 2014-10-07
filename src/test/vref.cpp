#include <xUnit++/xUnit++.h>
#include <entity/vref.hpp>
#include <entity/json.hpp>

using namespace std;
using namespace ent;


enum class EnumTest
{
	Zero,
	One,
	Two
};


SUITE("VRef Tests")
{
	json c;
	stack<int> stack;

	FACT("Can create a map to integer types")
	{
		os dst;
		int integer = 42;
		auto imap	= vref<int>(integer);

		imap.encode(c, dst, "a", stack);
		Assert.Equal("\"a\":42", dst.str());

		imap.decode(c, "1", 0, 0);
		Assert.Equal(1, integer);
	}


	FACT("Can create a map to floating-point types")
	{
		os dst;
		double floating	= 3.14;
		auto fmap		= vref<double>(floating);

		fmap.encode(c, dst, "a", stack);
		Assert.Equal("\"a\":3.14", dst.str());

		fmap.decode(c, "1.01", 0, 0);
		Assert.Equal(1.01, floating);
	}


	FACT("Can create a map to string types")
	{
		os dst;
		string text	= "something";
		auto smap	= vref<string>(text);

		smap.encode(c, dst, "a", stack);
		Assert.Equal("\"a\":\"something\"", dst.str());

		smap.decode(c, "\"something else\"", 0, 0);
		Assert.Equal("something else", text);
	}


	FACT("Can create a map to boolean types")
	{
		os dst;
		bool flag	= true;
		auto bmap	= vref<bool>(flag);

		bmap.encode(c, dst, "a", stack);
		Assert.Equal("\"a\":true", dst.str());

		bmap.decode(c, "false", 0, 0);
		Assert.False(flag);
	}


	FACT("Can create a map to enumerated types")
	{
		os dst;
		EnumTest e 		= EnumTest::One;
		auto emap		= vref<EnumTest>(e);

		emap.encode(c, dst, "a", stack);
		Assert.Equal("\"a\":1", dst.str());

		emap.decode(c, "2", 0, 0);
		Assert.Equal(EnumTest::Two, e);
	}


	FACT("Mapping from an incorrect value type uses the default value for that type")
	{
		int integer		= 42;
		auto imap		= vref<int>(integer);

		imap.decode(c, "\"invalid\"", 0, 0);
		Assert.Equal(0, integer);
	}


	FACT("Can create a map to an array of simple types")
	{
		os dst;
		vector<int> items 	= { 1, 1, 2, 3 };
		auto amap			= vref<decltype(items)>(items);

		amap.encode(c, dst, "a", stack);
		Assert.Equal("\"a\":[1,1,2,3]", dst.str());

		amap.decode(c, "[3,2,1]", 0, 0);
		Assert.Equal(3, items.size());
		Assert.Equal(1, items[2]);
	}


	FACT("Can create a map to a dictionary of simple types")
	{
		os dst;
		map<string, int> items	= { { "one", 1 }, { "two", 2 } };
		auto mmap				= vref<decltype(items)>(items);

		mmap.encode(c, dst, "a", stack);
		Assert.Equal("\"a\":{\"one\":1,\"two\":2}", dst.str());

		mmap.decode(c, "{\"three\":3}", 0, 0);
		Assert.Equal(1, items.size());
		Assert.Equal(3, items["three"]);
	}


	FACT("Can create a map to an array of enums")
	{
		os dst;
		vector<EnumTest> items	= { EnumTest::Two, EnumTest::Zero };
		auto amap				= vref<decltype(items)>(items);

		amap.encode(c, dst, "a", stack);
		Assert.Equal("\"a\":[2,0]", dst.str());

		amap.decode(c, "[0,1]", 0, 0);
		Assert.Equal(EnumTest::One, items[1]);
	}


	FACT("Can create a map to a dictionary of enums")
	{
		os dst;
		map<string, EnumTest> items	= { { "one", EnumTest::One }, { "two", EnumTest::Two } };
		auto mmap					= vref<decltype(items)>(items);

		mmap.encode(c, dst, "a", stack);
		Assert.Equal("\"a\":{\"one\":1,\"two\":2}", dst.str());

		mmap.decode(c, "{\"zero\":0}", 0, 0);
		Assert.Equal(1, items.size());
		Assert.Equal(EnumTest::Zero, items["zero"]);
	}


	FACT("Can create a map to a binary blob")
	{
		os dst;
		vector<byte> binary	= { 0x00, 0x01, 0x02, 0x88, 0xff };
		auto bmap			= vref<vector<byte>>(binary);

		bmap.encode(c, dst, "a", stack);
		Assert.Equal("\"a\":\"AAECiP8=\"", dst.str());

		bmap.decode(c, "\"Zm9vYmFy\"", 0, 0);
		Assert.Equal(vector<byte> { 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72 }, binary);
	}
}
