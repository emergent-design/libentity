#include <xUnit++/xUnit++.h>
#include <entity/utilities.hpp>

using namespace std;
using namespace ent;


SUITE("Utility Tests")
{
	// Based on the RF 4648 test vectors where "foobar" = 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72
	vector<tuple<string, vector<byte>>> base64_vectors()
	{
		return {
			make_tuple("",			vector<byte> {}),
			make_tuple("Zg==",		vector<byte> { 0x66 }),
			make_tuple("Zm8=",		vector<byte> { 0x66, 0x6f }),
			make_tuple("Zm9v",		vector<byte> { 0x66, 0x6f, 0x6f }),
			make_tuple("Zm9vYg==",	vector<byte> { 0x66, 0x6f, 0x6f, 0x62 }),
			make_tuple("Zm9vYmE=",	vector<byte> { 0x66, 0x6f, 0x6f, 0x62, 0x61 }),
			make_tuple("Zm9vYmFy",	vector<byte> { 0x66, 0x6f, 0x6f, 0x62, 0x61, 0x72 })
		};
	}


	DATA_THEORY("Base64 encoder test vectors", (string expected, vector<byte> value), base64_vectors)
	{
		Assert.Equal(expected, base64::encode(value));
	}


	DATA_THEORY("Base64 decoder test vectors", (string value, vector<byte> expected), base64_vectors)
	{
		Assert.Equal(expected, base64::decode(value));
	}
}
