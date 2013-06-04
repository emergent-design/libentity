#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#ifndef byte
	typedef unsigned char byte;
#endif


namespace ent
{
	std::string encode64(const std::vector<byte> &value);
	std::vector<byte> decode64(const std::string &value);
}
