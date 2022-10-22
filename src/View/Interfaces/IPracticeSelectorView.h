#pragma once
#include <vector>
#include <string>

class IPracticeSelectorView
{
public:
	virtual void setPracticeList(const std::vector<std::string>& practiceList) = 0;
	virtual void setDbPath(const std::string& dbPath) = 0;
	virtual void closeDialog() = 0;
};