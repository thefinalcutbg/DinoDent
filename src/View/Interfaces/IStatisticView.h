#pragma once

#include <vector>
#include <string>

class StatisticPresenter;

class IStatisticView
{
public:
	virtual void setPresenter(StatisticPresenter* presenter) = 0;
	virtual void setStatList(const std::vector<std::pair<std::string, int>>& stat) = 0;
};