#include "StatisticPresenter.h"
#include "View/Interfaces/ITabView.h"
#include "View/Interfaces/IStatisticView.h"
#include "StatisticDialogPresenter.h"
#include "Database/DbStatistics.h"



StatisticPresenter::StatisticPresenter(ITabView* tabView, long long rowId) :
	TabInstance(tabView, TabType::Statistic, nullptr),
	view{ tabView->statisticView() },
	year{ Date::currentDate().year - 1 }

{
	view->setPresenter(this);

}

std::vector<std::pair<std::string, int>> StatisticPresenter::getStatList()
{
	std::vector<std::pair<std::string, int>> result;

	for (auto& stat : m_statistics) {
		result.push_back({ stat.name, stat.count });
	}

	return result;
}

void StatisticPresenter::addStatistic()
{
	StatisticDialogPresenter p;
	auto result = p.openDialog();

	if (!result) return;

	result->count = DbStat::count(*result, year);
	
	m_statistics.push_back(*result);

	view->setStatList(getStatList());
}

void StatisticPresenter::removeStatistic(int idx)
{
	if (idx < 0 || idx >= m_statistics.size()) return;

	m_statistics.erase(m_statistics.begin()+idx);

	view->setStatList(getStatList());
}

void StatisticPresenter::yearChanged(int year)
{
	this->year = year;

	for (auto& stat : m_statistics) {
		stat.count = DbStat::count(stat, year);
	}

	view->setStatList(getStatList());
}


void StatisticPresenter::setDataToView()
{
	view->setYear(year);
	view->setStatList(getStatList());
}

bool StatisticPresenter::isNew()
{
	return false;
}

TabName StatisticPresenter::getTabName()
{
	return TabName{
		.header = "Статистики"
	};
}

bool StatisticPresenter::save()
{
	return true;
}

void StatisticPresenter::print()
{
}

long long StatisticPresenter::rowID() const
{
	return 0;
}
