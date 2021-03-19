#pragma once
#include <string>
#include <array>

class Validator
{

public:
	virtual bool validate(const std::string& text) { return true; };
	virtual bool validate(int begin, int end) { return true; };
	virtual bool validate(const std::array<bool, 6>& surfaces) { return true; }
};