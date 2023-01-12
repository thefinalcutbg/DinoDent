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

	ui.statisticTable->setStatisticLayout();

	connect(ui.yearSpin, &QSpinBox::valueChanged, [=](int value) {
			if (presenter) presenter->yearChanged(value);
		});


	connect(ui.addStat, &QPushButton::clicked, [=] {if (presenter) presenter->addStatistic();});

	connect(ui.deleteStat, &QAbstractButton::clicked,
		[=] {

			if (!presenter) return;

			int currentIdx = ui.statisticTable->selectedRow();
			int lastIdx = ui.statisticTable->verticalHeader()->count() - 1;

			//			if (currentIdx == -1) return;

			presenter->removeStatistic(currentIdx);

			if (currentIdx == lastIdx)
			{
				ui.statisticTable->selectRow(currentIdx - 1);
			}
			else ui.statisticTable->selectRow(currentIdx);
		});
}

void StatisticView::setStatList(const std::vector<std::pair<std::string, int>>& stat)
{
	m_model.setRows(stat);
}

StatisticView::~StatisticView()
{}

void StatisticView::setYear(int year)
{
	QSignalBlocker b{ ui.yearSpin };

	ui.yearSpin->setValue(year);
}
