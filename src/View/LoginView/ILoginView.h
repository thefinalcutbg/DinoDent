#pragma once

#include <vector>
#include <string>

class ILoginView
{
public:
	virtual void setPracticeNames(const std::vector<std::string>& practiceList) = 0;
	virtual void closeLogin() = 0;
};