#include "StatisticPresenter.h"
#include "View/Interfaces/ITabView.h"
#include "View/Interfaces/IStatisticView.h"
#include "StatisticDialogPresenter.h"

StatisticPresenter::StatisticPresenter(ITabView* tabView, long long rowId) :
	TabInstance (tabView, TabType::Statistic, nullptr),
	view{ tabView->statisticView() }
{
	view->setPresenter(this);
}

void StatisticPresenter::addStatistic()
{
	StatisticDialogPresenter p;
	p.openDialog();
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
