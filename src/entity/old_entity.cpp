#include "entity/old_entity.h"

using namespace std;

const static char lookup[]	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const static char pad		= '=';

string old_entity::encode(const vector<byte> &value)
{
	int i;
	long block;
	string result;

	int size	= value.size();
	int steps	= size / 3;
	int remain	= size % 3;
	auto *b		= value.data();

	result.reserve((steps + remain > 0) * 4);

	for (i=0; i<steps; i++)
	{
		block  = *b++ << 16;
		block += *b++ << 8;
		block += *b++;
		result.append(1, lookup[(block & 0xfc0000) >> 18]);
		result.append(1, lookup[(block & 0x03f000) >> 12]);
		result.append(1, lookup[(block & 0x000fc0) >> 6	]);
		result.append(1, lookup[(block & 0x00003f)		]);
	}

	if (remain)
	{
		block  = *b++ << 16;
		block += remain == 2 ? *b++ << 8 : 0;
		result.append(1, lookup[(block & 0xfc0000) >> 18]);
		result.append(1, lookup[(block & 0x03f000) >> 12]);
		result.append(1, remain == 2 ? lookup[(block & 0x000fc0) >> 6 ] : pad);
		result.append(1, pad);
	}

	return result;
}


vector<byte> old_entity::decode(const string &value)
{
	if (value.length() % 4) throw std::runtime_error("Invalid string for decoding as base64");

	int i, j;
	long block	= 0;
	int size	= value.length();
	int steps	= size / 4;
	int padding	= size ? (value[size-1] == pad) + (value[size-2] == pad) : 0;
	auto *c		= value.data();

	vector<byte> result;
	result.reserve(steps * 3 - padding);

	for (i=0; i<steps; i++, block=0)
	{
		for (j=0; j<4; j++, c++)
		{
			block <<= 6;

			if		(*c >= 0x41 && *c <= 0x5a)	block |= *c - 0x41;
			else if (*c >= 0x61 && *c <= 0x7a)	block |= *c - 0x47;
			else if (*c >= 0x30 && *c <= 0x39)	block |= *c + 0x04;
			else if (*c == 0x2b)				block |= 0x3e;
			else if (*c == 0x2f)				block |= 0x3f;
			else if (*c == pad) switch(padding)
			{
				case 1: result.push_back((block >> 16) & 0xff);
						result.push_back((block >> 8) & 0xff);
						return result;

				case 2: result.push_back((block >> 10) & 0xff);
						return result;

				default: throw runtime_error("Invalid padding in base64 string");
			}
		}

		result.push_back((block >> 16) & 0xff);
		result.push_back((block >> 8) & 0xff);
		result.push_back(block & 0xff);
	}

	return result;
}
