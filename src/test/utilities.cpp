#include "catch.hpp"
#include <entity/utilities.hpp>
#include <map>

using namespace std;
using namespace ent;


TEST_CASE("base64 test vectors", "[utilities]")
{
	map<string, vector<byte>> test_vectors = {
		{ "",			vector<byte> {}},
		{ "Zg==",		vector<byte> { 0x66 }},
		{ "Zm8=",		vector<byte> { 0x66, 0x6f }},
		{ "Zm9v",		vector<byte> { 0x66, 0x6f, 0x6f }},
		{ "Zm9vYg==",	vector<byte> { 0x66, 0x6f, 0x6f, 0x62 }},
		{ "Zm9vYmE=",	vector<byte> { 0x66, 0x6f, 0x6f, 0x62, 0x61 }},
		{ "Zm9vYmFy",	vector<byte> { 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72 }}
	};

	SECTION("encoder")
	{
		for (auto &i : test_vectors)
		{
			REQUIRE(base64::encode(i.second) == i.first);
		}
	}

	SECTION("decoder")
	{
		for (auto &i : test_vectors)
		{
			REQUIRE(base64::decode(i.first) == i.second);
		}
	}
}

