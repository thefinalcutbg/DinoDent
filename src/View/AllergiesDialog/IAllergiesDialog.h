#pragma once
#include <string>

class IAllergiesDialog
{
public:
	virtual void open(std::string allergies, std::string current, std::string past) = 0;
	virtual void close() = 0;
};