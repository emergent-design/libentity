#include "doctest.h"
#include <entity/vref/vref.hpp>
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

	bool operator==(const Test &other) const
	{
		return this->a == other.a
			&& this->b == other.b
			&& this->c == other.c
			&& this->d == other.d
			&& this->sub.a == other.sub.a;
	}
};


TEST_CASE("vref can map a reference to different types") //, "[vref]")
{
	json c;
	os dst;
	stack<int> stack;

	SUBCASE("can map to integer types")
	{
		int integer = 42;

		make_vref(integer).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":42");

		make_vref(integer).decode(c, "1", 0, 0);
		CHECK(integer == 1);
	}


	SUBCASE("can map to a const when reading")
	{
		make_vref(42).encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":42");

		// make_vref(42).decode(c, "1", 0, 0);
	}


	SUBCASE("can map to floating-point types")
	{
		double floating	= 3.14;

		make_vref(floating).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":3.14");

		make_vref(floating).decode(c, "1.01", 0, 0);
		CHECK(floating == 1.01);
	}


	SUBCASE("can map to string types")
	{
		string text	= "something";

		make_vref(text).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":\"something\"");

		make_vref(text).decode(c, "\"something else\"", 0, 0);
		CHECK(text == "something else");
	}


	SUBCASE("can map to const string types")
	{
		make_vref("something const"s).encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":\"something const\"");
	}

#if __has_include(<filesystem>)
	SUBCASE("can map to paths")
	{
		std::filesystem::path path = "/tmp/something.txt";

		make_vref(path).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":\"/tmp/something.txt\"");

		make_vref(path).decode(c, "\"/a/path/to/somewhere\"", 0, 0);
		CHECK(path == "/a/path/to/somewhere");
	}

	SUBCASE("can map to const paths")
	{
		make_vref(std::filesystem::path { "/tmp" } / "file.txt").encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":\"/tmp/file.txt\"");
	}
#endif

	SUBCASE("can map to boolean types")
	{
		bool flag	= true;

		make_vref(flag).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":true");

		make_vref(flag).decode(c, "false", 0, 0);
		CHECK_FALSE(flag);
	}


	SUBCASE("can map to const boolean types")
	{
		make_vref(true).encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":true");
	}


	SUBCASE("can map to enumerated types")
	{
		EnumTest e = EnumTest::One;

		make_vref(e).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":1");

		make_vref(e).decode(c, "2", 0, 0);
		CHECK(e == EnumTest::Two);
	}


	SUBCASE("can map to const enumerated types")
	{
		make_vref(EnumTest::Two).encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":2");
	}


	SUBCASE("mapping from an incorrect value type uses the default value for that type")
	{
		int integer	= 42;

		make_vref(integer).decode(c, "\"invalid\"", 0, 0);
		CHECK(integer == 0);
	}


	SUBCASE("can map to a vector of simple types")
	{
		vector<int> items = { 1, 1, 2, 3 };

		make_vref(items).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":[1,1,2,3]");

		items.clear();
		make_vref(items).decode(c, "[3,2,1]", 0, 0);
		CHECK(items.size() == 3);
		CHECK(items[2] == 1);
	}

	SUBCASE("can map to a const vector of simple types")
	{
		vref<const vector<int>>({ 3, 2, 1}).encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":[3,2,1]");
	}


	SUBCASE("can map to a set of simple types")
	{
		set<int> items	= { 2, 4, 8 };

		make_vref(items).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":[2,4,8]");

		make_vref(items).decode(c, "[3,3,2,1]", 0, 0);
		CHECK(items.size() == 3);
		CHECK(items.count(2) == 1);
	}


	SUBCASE("can map to a const set of simple types")
	{
		vref<const set<int>>({ 3, 2, 1}).encode(c, dst, "a", stack);

		// They'll end up in order due to being in a set
		CHECK(dst.str() == "\"a\":[1,2,3]");
	}


	SUBCASE("can map to a set of strings")
	{
		set<string> items = { "2", "4", "8" };

		make_vref(items).encode(c, dst, "a", stack);
		CHECK(dst.str() == R"json("a":["2","4","8"])json");

		make_vref(items).decode(c, R"json(["3","3","2","1"])json", 0, 0);
		CHECK(items.size() == 3);
		CHECK(items.count("2") == 1);
	}


	SUBCASE("can map to a const set of strings")
	{
		vref<const set<string>>({ "8", "4", "2" }).encode(c, dst, "a", stack);

		// They'll end up in order due to being in a set
		CHECK(dst.str() == R"json("a":["2","4","8"])json");
	}


	SUBCASE("can map to a dictionary of simple types")
	{
		map<string, int> items	= { { "one", 1 }, { "two", 2 } };

		make_vref(items).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":{\"one\":1,\"two\":2}");

		items.clear();
		make_vref(items).decode(c, "{\"three\":3}", 0, 0);
		CHECK(items.size() == 1);
		CHECK(items["three"] == 3);
	}


	SUBCASE("can map to a const dictionary of simple types")
	{
		vref<const map<string, int>>({ { "one", 1 }, { "two", 2 }}).encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":{\"one\":1,\"two\":2}");
	}


	SUBCASE("can map to a vector of enums")
	{
		vector<EnumTest> items	= { EnumTest::Two, EnumTest::Zero };

		make_vref(items).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":[2,0]");

		make_vref(items).decode(c, "[0,1]", 0, 0);
		CHECK(items[1] == EnumTest::One);
	}

	SUBCASE("can map to a const vector of enums")
	{
		vref<const vector<EnumTest>>({ EnumTest::Two, EnumTest::Zero }).encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":[2,0]");
	}


	SUBCASE("can map to a dictionary of enums")
	{
		map<string, EnumTest> items	= { { "one", EnumTest::One }, { "two", EnumTest::Two } };

		make_vref(items).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":{\"one\":1,\"two\":2}");

		items.clear();
		make_vref(items).decode(c, "{\"zero\":0}", 0, 0);
		CHECK(items.size() == 1);
		CHECK(items["zero"] == EnumTest::Zero);
	}

	SUBCASE("can map to a const dictionary of enums")
	{
		vref<const map<string, EnumTest>>({
			{ "one", EnumTest::One },
			{ "two", EnumTest::Two }
		}).encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":{\"one\":1,\"two\":2}");
	}


	SUBCASE("can map to a binary blob")
	{
		vector<uint8_t> binary = { 0x00, 0x01, 0x02, 0x88, 0xff };

		make_vref(binary).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":\"AAECiP8=\"");

		make_vref(binary).decode(c, "\"Zm9vYmFy\"", 0, 0);
		CHECK(binary == vector<uint8_t>({ 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72 }));
	}


	SUBCASE("can map to a const binary blob")
	{
		vref<const vector<uint8_t>>({ 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72 }).encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":\"Zm9vYmFy\"");
	}


	SUBCASE("can map to a tree")
	{
		tree t = tree().set("name", "tree").set("number", 42);

		make_vref(t).encode(c, dst, "t", stack);
		CHECK(dst.str() == "\"t\":{\"name\":\"tree\",\"number\":42}");

		make_vref(t).decode(c, "{\"value\": 1234 }", 0, 0);
		CHECK(t["value"].as_long() == 1234);
	}


	SUBCASE("can map to a const tree")
	{
		vref<const tree>(tree().set("name", "tree").set("number", 42)).encode(c, dst, "t", stack);

		CHECK(dst.str() == "\"t\":{\"name\":\"tree\",\"number\":42}");
	}


	SUBCASE("can map to an array")
	{
		array<int, 4> a = {{ 2, 4, 5, 6 }};

		make_vref(a).encode(c, dst, "a", stack);
		CHECK(dst.str() == "\"a\":[2,4,5,6]");

		make_vref(a).decode(c, "[1,2,3,4]", 0, 0);
		CHECK(a[3] == 4);

		// If there are too many items in the JSON then only take the first N
		make_vref(a).decode(c, "[8,7,6,5,4,3,2,1]", 0, 0);
		CHECK(a[3] == 5);
	}


	SUBCASE("can map to a const array")
	{
		vref<const array<int, 4>>({ 2, 4, 5, 6 }).encode(c, dst, "a", stack);

		CHECK(dst.str() == "\"a\":[2,4,5,6]");
	}


	SUBCASE("can map to an entity")
	{
		Test test;

		make_vref(test).encode(c, dst, "a", stack);
		CHECK(dst.str() == R"json("a":{"a":8,"b":42,"c":0,"d":"some","sub":{"a":0}})json");

		make_vref(test).decode(c, R"json({"a":4,"b":-1,"c":8,"d":"other","sub":{"a":1}})json", 0, 0);
		CHECK(test.a == 4);
		CHECK(test.b == -1);
		CHECK(test.c == 8);
		CHECK(test.d == "other");
		CHECK(test.sub.a == 1);

	}


	SUBCASE("can map to a const entity")
	{
		const Test t;

		make_vref(t).encode(c, dst, "a", stack);

		CHECK(dst.str() == R"json("a":{"a":8,"b":42,"c":0,"d":"some","sub":{"a":0}})json");


		make_vref(t).decode(c, R"json({"a":4,"b":-1,"c":8,"d":"other","sub":{"a":1}})json", 0, 0);

		// The decode has no effect on a const entity
		CHECK(t.a == 8);
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


	SUBCASE("can check for circular references when smart pointers are involved")
	{
		struct Root
		{
			std::shared_ptr<Root> child;

			emap(eref(child))
		};

		// With a null pointer in child it should not throw
		auto root = std::make_shared<Root>();
		CHECK_NOTHROW(make_vref(root).encode(c, dst, "a", stack));

		// If child refers to itself then it should fail
		root->child = root;
		CHECK_THROWS(make_vref(root).encode(c, dst, "a", stack));

		// If child is a different object then it should not throw
		root->child = std::make_shared<Root>();
		CHECK_NOTHROW(make_vref(root).encode(c, dst, "a", stack));

		// If the child of the child points back to the root then it should fail
		root->child->child = root;
		CHECK_THROWS(make_vref(root).encode(c, dst, "a", stack));
	}
}




#include <typeindex>
#include <any>

struct TypeCheck
{
	std::any in;		// Value to encode
	std::any out;		// Value expected when decoding
	std::string encode;	// Expected encoding of "in"
	std::string decode;	// Encoding to produce "out"
};

const std::map<std::type_index, TypeCheck> PointerChecks = {
	{ std::type_index(typeid(bool)), {
		true, false,
		R"json("a":true)json", "false"
	}},
	{ std::type_index(typeid(int)), {
		42, 13,
		R"json("a":42)json", "13"
	}},
	{ std::type_index(typeid(double)), {
		3.14, 1.2,
		R"json("a":3.14)json", "1.2"
	}},
	{ std::type_index(typeid(Test)), {
		Test(), Test { .a = 4, .b = -1, .c = 8, .d = "other", .sub={ .a = 1 } },
		R"json("a":{"a":8,"b":42,"c":0,"d":"some","sub":{"a":0}})json",
		R"json({"a":4,"b":-1,"c":8,"d":"other","sub":{"a":1}})json"
	}},
	{ std::type_index(typeid(std::vector<Test>)), {
		std::vector<Test>(), std::vector<Test>(1),
		R"json("a":[])json",
		R"json([{"a":8,"b":42,"c":0,"d":"some","sub":{"a":0}}])json"
	}}
};


TEST_CASE_TEMPLATE("vref can map to shared pointers of encodable types", T, bool, int, double, Test, std::vector<Test>)
{
	json c;
	os dst;
	stack<int> stack;

	SUBCASE("can map to a null shared pointer")
	{
		std::shared_ptr<T> test;

		make_vref(test).encode(c, dst, "a", stack);

		CHECK(dst.str() == R"json("a":null)json");

		make_vref(test).decode(c, R"json(null)json", 0, 0);

		CHECK(test == nullptr);
	}

	SUBCASE("can map to a valid shared pointer")
	{
		// auto test = std::make_shared<T>();
		const auto check = PointerChecks.at(std::type_index(typeid(T)));

		auto test = std::make_shared<T>(std::any_cast<T>(check.in));

		make_vref(test).encode(c, dst, "a", stack);

		CHECK(dst.str() == check.encode);

		make_vref(test).decode(c, check.decode, 0, 0);

		CHECK(*test == std::any_cast<T>(check.out));
	}
}

// const pointers ???


TEST_CASE_TEMPLATE("vref can map to unique pointers of encodable types", T, bool, int, double, Test, std::vector<Test>)
{
	json c;
	os dst;
	stack<int> stack;

	SUBCASE("can map to a null shared pointer")
	{
		std::unique_ptr<T> test;

		make_vref(test).encode(c, dst, "a", stack);

		CHECK(dst.str() == R"json("a":null)json");

		make_vref(test).decode(c, R"json(null)json", 0, 0);

		CHECK(test == nullptr);
	}

	SUBCASE("can map to a valid shared pointer")
	{
		// auto test = std::make_shared<T>();
		const auto check = PointerChecks.at(std::type_index(typeid(T)));

		auto test = std::make_unique<T>(std::any_cast<T>(check.in));

		make_vref(test).encode(c, dst, "a", stack);

		CHECK(dst.str() == check.encode);

		make_vref(test).decode(c, check.decode, 0, 0);

		CHECK(*test == std::any_cast<T>(check.out));
	}
}


