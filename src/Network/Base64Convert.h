#pragma once
#include <string>

namespace Base64Convert
{
	std::string encode(const std::string& input);
	std::string decode(const std::string& input);
};

