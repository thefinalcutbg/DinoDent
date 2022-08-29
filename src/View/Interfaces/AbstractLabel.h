#pragma once
#include <string>

class AbstractLabel
{
public:
	virtual void setLabelText(const std::string &text) = 0;
};

