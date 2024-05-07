#include "doctest.h"
#include <entity/utilities/base64.hpp>
#include <entity/utilities/compare.hpp>
#include <map>

using namespace std;
using namespace ent;

TEST_SUITE("utilities")
{
	TEST_CASE("base64 test vectors")
	{
		map<string, vector<uint8_t>> test_vectors = {
			{ "",			vector<uint8_t> {}},
			{ "Zg==",		vector<uint8_t> { 0x66 }},
			{ "Zm8=",		vector<uint8_t> { 0x66, 0x6f }},
			{ "Zm9v",		vector<uint8_t> { 0x66, 0x6f, 0x6f }},
			{ "Zm9vYg==",	vector<uint8_t> { 0x66, 0x6f, 0x6f, 0x62 }},
			{ "Zm9vYmE=",	vector<uint8_t> { 0x66, 0x6f, 0x6f, 0x62, 0x61 }},
			{ "Zm9vYmFy",	vector<uint8_t> { 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72 }}
		};

		SUBCASE("encoder")
		{
			for (auto &i : test_vectors)
			{
				CHECK(base64::encode(i.second) == i.first);
			}
		}

		SUBCASE("decoder")
		{
			for (auto &i : test_vectors)
			{
				CHECK(base64::decode(i.first) == i.second);
			}
		}
	}

	TEST_CASE("trees and entities can be compared")
	{
		SUBCASE("trees")
		{
			const auto before = tree()
				.set("string", "string value")
				.set("integer", 42)
				.set("floating", 3.14)
				.set("boolean", true)
				.set("array", vector<tree> { 1, 2, 3 })
				.set("tree", tree().set("name", "child"));

			const auto after = tree(before)
				.set("integer", 8)
				.set("only", "here")
				.set("tree", tree().set("name", "another"));

			auto diffs = compare::trees(before, after);

			CHECK(diffs.size()					== 2);
			CHECK(diffs[0].level				== "integer");
			CHECK(diffs[0].before.as_long() 	== 42);
			CHECK(diffs[0].after.as_long()		== 8);
			CHECK(diffs[1].level				=="tree:name");
			CHECK(diffs[1].before.as_string()	== "child");
			CHECK(diffs[1].after.as_string()	== "another");
		}


		SUBCASE("entities")
		{
			struct Entity
			{
				string name			= "simple";
				bool flag			= true;
				int integer			= 42;

				emap(eref(name), eref(flag), eref(integer))
			};

			auto diffs = compare::entities(Entity(), Entity { "changed", false, 42 });

			CHECK(diffs.size() == 2);
			CHECK(diffs[0].level				== "flag");
			CHECK(diffs[0].before.as_bool()		== true);
			CHECK(diffs[0].after.as_bool()		== false);
			CHECK(diffs[1].level				== "name");
			CHECK(diffs[1].before.as_string() 	== "simple");
			CHECK(diffs[1].after.as_string()	== "changed");
		}
	}
}
