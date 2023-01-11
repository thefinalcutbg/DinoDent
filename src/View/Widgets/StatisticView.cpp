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

	ui.statisticTable->setModel(&m_model);

	connect(ui.addStat, &QPushButton::clicked, [=] {if (presenter) presenter->addStatistic();});

	auto dateChanged = [=] {

		auto f = ui.dateFromEdit->date();
		auto t = ui.dateToEdit->date();

		presenter->dateRangeChanged(
			{ f.day(), f.month(), f.year() },
			{ t.day(), t.month(), t.year() }
		);
	};

	connect(ui.dateFromEdit, &QDateEdit::dateChanged, [=] {dateChanged();});
	connect(ui.dateToEdit, &QDateEdit::dateChanged, [=] {dateChanged();});
}

void StatisticView::setStatList(const std::vector<std::pair<std::string, int>>& stat)
{
	m_model.setRows(stat);
}

StatisticView::~StatisticView()
{}
