#include "doctest.h"
#include <entity/vref.hpp>
#include <entity/json.hpp>
#include <entity/entity.hpp>

using namespace std;
using namespace ent;


enum class EnumTest
{
	Zero,
	One,
	Two
};


TEST_CASE("vref can map a reference to different types") //, "[vref]")
{
	json c;
	os dst;
	stack<int> stack;

	SUBCASE("can map to integer types")
	{
		int integer = 42;
		auto imap	= vref<int>(integer);

		imap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":42");


		imap.decode(c, "1", 0, 0);
		CHECK(integer == 1);
	}


	SUBCASE("can map to floating-point types")
	{
		double floating	= 3.14;
		auto fmap		= vref<double>(floating);

		fmap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":3.14");

		fmap.decode(c, "1.01", 0, 0);
		CHECK(floating == 1.01);
	}


	SUBCASE("can map to string types")
	{
		string text	= "something";
		auto smap	= vref<string>(text);

		smap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":\"something\"");

		smap.decode(c, "\"something else\"", 0, 0);
		CHECK(text == "something else");
	}


	SUBCASE("can map to boolean types")
	{
		bool flag	= true;
		auto bmap	= vref<bool>(flag);

		bmap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":true");

		bmap.decode(c, "false", 0, 0);
		CHECK_FALSE(flag);
	}


	SUBCASE("can map to enumerated types")
	{
		EnumTest e 	= EnumTest::One;
		auto emap	= vref<EnumTest>(e);

		emap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":1");

		emap.decode(c, "2", 0, 0);
		CHECK(e == EnumTest::Two);
	}


	SUBCASE("mapping from an incorrect value type uses the default value for that type")
	{
		int integer	= 42;
		auto imap	= vref<int>(integer);

		imap.decode(c, "\"invalid\"", 0, 0);
		CHECK(integer == 0);
	}


	SUBCASE("can map to a vector of simple types")
	{
		vector<int> items 	= { 1, 1, 2, 3 };
		auto amap			= vref<decltype(items)>(items);

		amap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":[1,1,2,3]");

		items.clear();
		amap.decode(c, "[3,2,1]", 0, 0);
		CHECK(items.size() == 3);
		CHECK(items[2] == 1);
	}


	SUBCASE("can map to a set of simple types")
	{
		set<int> items	= { 2, 4, 8 };
		auto amap		= vref<decltype(items)>(items);

		amap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":[2,4,8]");

		amap.decode(c, "[3,3,2,1]", 0, 0);
		CHECK(items.size() == 3);
		CHECK(items.count(2) == 1);
	}


	// SUBCASE("can map to a set of strings")
	// {
	// 	set<string> items	= { "2", "4", "8" };
	// 	auto amap			= vref<decltype(items)>(items);

	// 	amap.encode(c, dst, "a", stack);
	// 	CHECK(dst.str() == R"json("a":["2","4","8"])json");

	// 	amap.decode(c, R"json(["3","3","2","1"])json", 0, 0);
	// 	CHECK(items.size() == 3);
	// 	CHECK(items.count("2") == 1);
	// }


	SUBCASE("can map to a dictionary of simple types")
	{
		map<string, int> items	= { { "one", 1 }, { "two", 2 } };
		auto mmap				= vref<decltype(items)>(items);

		mmap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":{\"one\":1,\"two\":2}");

		items.clear();
		mmap.decode(c, "{\"three\":3}", 0, 0);
		CHECK(items.size() == 1);
		CHECK(items["three"] == 3);
	}


	SUBCASE("can map to a vector of enums")
	{
		vector<EnumTest> items	= { EnumTest::Two, EnumTest::Zero };
		auto amap				= vref<decltype(items)>(items);

		amap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":[2,0]");

		amap.decode(c, "[0,1]", 0, 0);
		CHECK(items[1] == EnumTest::One);
	}


	SUBCASE("can map to a dictionary of enums")
	{
		map<string, EnumTest> items	= { { "one", EnumTest::One }, { "two", EnumTest::Two } };
		auto mmap					= vref<decltype(items)>(items);

		mmap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":{\"one\":1,\"two\":2}");

		items.clear();
		mmap.decode(c, "{\"zero\":0}", 0, 0);
		CHECK(items.size() == 1);
		CHECK(items["zero"] == EnumTest::Zero);
	}


	SUBCASE("can map to a binary blob")
	{
		vector<uint8_t> binary	= { 0x00, 0x01, 0x02, 0x88, 0xff };
		auto bmap				= vref<vector<uint8_t>>(binary);

		bmap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":\"AAECiP8=\"");

		bmap.decode(c, "\"Zm9vYmFy\"", 0, 0);
		CHECK(binary == vector<uint8_t>({ 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72 }));
	}


	SUBCASE("can map to a tree")
	{
		tree t = tree().set("name", "tree").set("number", 42);
		auto tmap	= vref<tree>(t);

		tmap.encode(c, dst, "t", stack);
		CHECK(dst.str() == "\"t\":{\"name\":\"tree\",\"number\":42}");

		tmap.decode(c, "{\"value\": 1234 }", 0, 0);
		CHECK(t["value"].as_long() == 1234);
	}


	SUBCASE("can map to an array")
	{
		array<int, 4> a = {{ 2, 4, 5, 6 }};
		auto amap		= vref<array<int, 4>>(a);

		amap.encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":[2,4,5,6]");

		amap.decode(c, "[1,2,3,4]", 0, 0);
		CHECK(a[3] == 4);

		// If there are too many items in the JSON then only take the first N
		amap.decode(c, "[8,7,6,5,4,3,2,1]", 0, 0);
		CHECK(a[3] == 5);
	}


	SUBCASE("can modify simple values via vref")
	{
		auto modifier = [](ent::any_ref ref) {
			if (ref.is<int64_t>())
			{
				ref.cast<int64_t>() += 1;
			}
			else if (ref.is<uint16_t>())
			{
				ref.cast<uint16_t>()--;
			}
			else if (ref.is<string>())
			{
				ref.cast<string>() += "thing";
			}
			else if (ref.is<double>())
			{
				ref.cast<double>() = M_PI;
			}
		};

		int a		= 8;
		int64_t b	= 42;
		uint16_t c	= 0;
		string d	= "some";
		double e	= 0;

		ent::make_vref(a).modify(modifier);
		ent::make_vref(b).modify(modifier);
		ent::make_vref(c).modify(modifier);
		ent::make_vref(d).modify(modifier);
		ent::make_vref(e).modify(modifier);

		CHECK(a == 8);
		CHECK(b == 43);
		CHECK(c == 0xffff);
		CHECK(d == "something");
		CHECK(e == M_PI);
	}


	SUBCASE("can modify a vector of values")
	{
		vector<int32_t> values = { 0, 1, 2, 3 };

		ent::make_vref(values).modify([](any_ref r) {
			auto &v = r.cast<int32_t>();
			v = be32toh(v);
		});

		CHECK(values == std::vector<int32_t> {
			0x00000000,
			0x01000000,
			0x02000000,
			0x03000000
		});
	}


	SUBCASE("casting an any_ref to the wrong type will throw")
	{
		int a = 0;
		CHECK_THROWS(
			ent::make_vref(a).modify(
				[](any_ref r) { r.cast<int64_t>()++; }
			)
		);
	}


	SUBCASE("can recursively modify an entity via vrefs")
	{
		struct Test
		{
			struct Sub
			{
				uint8_t a = 0;
				emap(eref(a));
			};

			int a		= 8;
			int64_t b	= 42;
			uint16_t c	= 0;
			string d	= "some";
			Sub sub;

			emap(eref(a), eref(b), eref(c), eref(d), eref(sub))
		};

		Test test;

		ent::make_vref(test).modify([](any_ref ref) {
			if (ref.is<int64_t>())
			{
				ref.cast<int64_t>()++;
			}
			else if (ref.is<uint8_t>())
			{
				ref.cast<uint8_t>()--;
			}
		});

		CHECK(test.a == 8);
		CHECK(test.b == 43);
		CHECK(test.c == 0);
		CHECK(test.d == "some");
		CHECK(test.sub.a == 0xff);
	}
}

