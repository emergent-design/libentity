#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstdint>

namespace ent
{
	// typedef std::ostringstream os;
	// typedef std::istringstream is;


	struct base64
	{
		static std::string encode(const std::vector<uint8_t> &value)
		{
			const static char lookup[]	= "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
			const static char pad		= '=';

			if (value.empty())
			{
				return{};
			}

			int i;
			long block;
			int size	= value.size();
			int steps	= size / 3;
			int remain	= size % 3;

			std::string result((steps + (remain > 0)) * 4, 0);

			auto *b		= value.data();
			auto *r		= &result.front();

			for (i=0; i<steps; i++)
			{
				block  = *b++ << 16;
				block += *b++ << 8;
				block += *b++;
				*r++ = lookup[(block & 0xfc0000) >> 18];
				*r++ = lookup[(block & 0x03f000) >> 12];
				*r++ = lookup[(block & 0x000fc0) >> 6];
				*r++ = lookup[(block & 0x00003f)];
			}

			if (remain)
			{
				block  = *b++ << 16;
				block += remain == 2 ? *b++ << 8 : 0;
				*r++ = lookup[(block & 0xfc0000) >> 18];
				*r++ = lookup[(block & 0x03f000) >> 12];
				*r++ = remain == 2 ? lookup[(block & 0x000fc0) >> 6 ] : pad;
				*r++ = pad;
			}

			return result;
		}


		static std::vector<uint8_t> decode(const std::string &value)
		{
			const static char pad = '=';

			if (value.empty())
			{
				return {};
			}

			if (value.length() % 4)
			{
				throw std::runtime_error("Invalid string for decoding as base64");
			}

			int i, j;
			long block	= 0;
			int size	= value.length();
			int steps	= size / 4;
			int padding	= size ? (value[size-1] == pad) + (value[size-2] == pad) : 0;

			std::vector<uint8_t> result(steps * 3 - padding);

			auto *c = value.data();
			auto *r = result.data();

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
						case 1: *r++ = (block >> 16) & 0xff;
								*r++ = (block >> 8) & 0xff;
								return result;

						case 2: *r++ = (block >> 10) & 0xff;
								return result;

						default: throw std::runtime_error("Invalid padding in base64 string");
					}
				}

				*r++ = (block >> 16) & 0xff;
				*r++ = (block >> 8) & 0xff;
				*r++ = block & 0xff;
			}

			return result;
		}
	};
}
