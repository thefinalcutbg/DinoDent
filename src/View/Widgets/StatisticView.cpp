#include "StatisticView.h"
#include "View/Theme.h"
StatisticView::StatisticView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.addStat->setHoverColor(Theme::mainBackgroundColor);
	ui.deleteStat->setHoverColor(Theme::mainBackgroundColor);

	ui.addStat->setIcon(QIcon(":/icons/icon_add.png"));
	ui.deleteStat->setIcon(QIcon(":/icons/icon_remove.png"));

	setStyleSheet(Theme::getFancyStylesheet());

	connect(ui.addStat, &QPushButton::clicked, [=] {if (presenter) presenter->addStatistic();});
}

StatisticView::~StatisticView()
{}
