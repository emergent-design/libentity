#include "doctest.h"
#include <entity/utilities.hpp>
#include <map>

using namespace std;
using namespace ent;


TEST_CASE("base64 test vectors") //, "[utilities]")
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

