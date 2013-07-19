#include <xUnit++/xUnit++.h>
#include <entity/entity.h>

using namespace std;
using namespace ent;


SUITE("Map Tests")
{
	FACT("Can create a map to numeric types")
	{
		int integer		= 42;
		double floating	= 3.14;
		value newValue	= 1.01;
		auto imap		= vmap<int>(integer);
		auto fmap		= vmap<double>(floating);

		Assert.Equal(42,	imap.to().number);
		Assert.Equal(3.14,	fmap.to().number);

		imap.from(newValue);
		fmap.from(newValue);

		Assert.Equal(1,		integer);
		Assert.Equal(1.01,	floating);
	}


	FACT("Can create a map to string types")
	{
		string text		= "something";
		value newValue	= "something else";
		auto smap		= vmap<string>(text);

		Assert.Equal("something", smap.to().string);

		smap.from(newValue);

		Assert.Equal("something else", text);
	}


	FACT("Can create a map to boolean types")
	{
		bool flag		= true;
		value newValue	= false;
		auto bmap		= vmap<bool>(flag);

		Assert.True(bmap.to().boolean);

		bmap.from(newValue);

		Assert.False(flag);
	}


	FACT("Mapping from an incorrect value type uses the default value for that type")
	{
		int integer		= 42;
		auto imap		= vmap<int>(integer);
		value newValue	= "invalid";
	
		imap.from(newValue);

		Assert.Equal(0, integer);
	}


	FACT("Can create a map to an array of simple types")
	{
		vector<int> items 		= { 1, 1, 2, 3 };
		vector<value> newItems	= { 3, 2, 1 };
		auto amap				= vmap<int>(items);
		value newValue			= newItems;

		Assert.Equal(2, amap.to().array[2].number);

		amap.from(newValue);

		Assert.Equal(3, items.size());
		Assert.Equal(1, items[2]);
	}


	FACT("Can create a map to a dictionary of simple types")
	{
		map<string, int> items	= { { "one", 1 }, { "two", 2 } };
		tree newItems			= tree().set("three", 3);
		auto mmap				= vmap<int>(items);
		value newValue			= make_shared<tree>(newItems);

		Assert.Equal(1, mmap.to().object->get<int>("one"));

		mmap.from(newValue);

		Assert.Equal(1, items.size());
		Assert.Equal(3, items["three"]);
	}
}
