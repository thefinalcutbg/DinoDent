#pragma once
#include "View/Interfaces/IStatisticView.h"
#include "TabInstance.h"
#include "Model/Date.h"
#include "Model/Dental/DentalStatistics.h"

class ITabView;

class StatisticPresenter : public TabInstance
{
	IStatisticView* view{ nullptr };

	std::vector<DentalStatistic> m_statistics;

	int year{ 0 };

	std::vector<std::pair<std::string, int>> getStatList();

public:
	StatisticPresenter(ITabView* tabView, long long rowId = 0);

	void addStatistic();
	void removeStatistic(int idx);
	void yearChanged(int year);
	// Inherited via TabInstance
	virtual void setDataToView() override;
	virtual bool isNew() override;
	virtual TabName getTabName() override;
	virtual bool save() override;
	virtual void print() override;
	virtual long long rowID() const override;
};