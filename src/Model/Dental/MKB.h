#pragma once
#include <string>

namespace MKB
{
	void initialize();
	const std::string& getNameFromMKBCode(const std::string& mkbCode);
	bool isInitialized();
}