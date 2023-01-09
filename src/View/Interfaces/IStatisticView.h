#pragma once

class StatisticPresenter;

class IStatisticView
{
public:
	virtual void setPresenter(StatisticPresenter* presenter) = 0;
};