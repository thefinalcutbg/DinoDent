#pragma once
#include <string>

namespace Base64Convert
{
	std::string encode(const char* in, size_t in_len);
	std::string decode(const std::string& input);
};

