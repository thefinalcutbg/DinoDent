#pragma once
#include "View/Interfaces/IStatisticView.h"
#include "TabInstance.h"
#include "Model/Date.h"
#include "Model/Dental/DentalStatistics.h"

class ITabView;

class StatisticPresenter : public TabInstance
{
	IStatisticView* view{ nullptr };

	std::vector<DentalStatistic> stats;

	Date date_from;
	Date date_to;

public:
	StatisticPresenter(ITabView* tabView, long long rowId = 0);

	void addStatistic();
	void dateRangeChanged(const Date& from, const Date& to);
	// Inherited via TabInstance
	virtual void setDataToView() override;
	virtual bool isNew() override;
	virtual TabName getTabName() override;
	virtual bool save() override;
	virtual void print() override;
	virtual long long rowID() const override;
};