#include "StatisticPresenter.h"
#include "View/Interfaces/ITabView.h"
#include "View/Interfaces/IStatisticView.h"
#include "StatisticDialogPresenter.h"
#include "Database/DbStatistics.h"

StatisticPresenter::StatisticPresenter(ITabView* tabView, long long rowId) :
	TabInstance(tabView, TabType::Statistic, nullptr),
	view{ tabView->statisticView() },
	date_from(Date::currentDate()),
	date_to(1, 1, date_from.year)
{
	view->setPresenter(this);

}

void StatisticPresenter::addStatistic()
{
	StatisticDialogPresenter p;
	auto result = p.openDialog();

	if (!result) return;

	result->count = DbStat::count(*result, date_from, date_to);
	
	stats.push_back(*result);

	std::vector<std::pair<std::string, int>> toView;

	for (auto stat : stats) {
		toView.push_back(std::make_pair(stat.name, stat.count));
	}

	view->setStatList(toView);
}

void StatisticPresenter::dateRangeChanged(const Date& from, const Date& to)
{
	date_from = from;
	date_to = to;

	std::vector<std::pair<std::string, int>> toView;

	for (auto stat : stats) {
		stat.count = DbStat::count(stat, from, to);
		toView.push_back(std::make_pair(stat.name, stat.count));
	}

	view->setStatList(toView);
	
}

void StatisticPresenter::setDataToView()
{
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
