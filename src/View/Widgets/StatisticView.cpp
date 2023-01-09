#include "StatisticView.h"
#include "View/Theme.h"
StatisticView::StatisticView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setStyleSheet(Theme::getFancyStylesheet());
}

StatisticView::~StatisticView()
{}
