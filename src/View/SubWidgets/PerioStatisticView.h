#pragma once

#include <QWidget>
#include "ui_PerioStatisticView.h"

class HexagonGraphicsItem;
class PerioStatistic;

class PerioStatisticView : public QWidget
{
	Q_OBJECT

	HexagonGraphicsItem* hexagonGraphicsItem;

	void paintEvent(QPaintEvent* event);

public:
	PerioStatisticView(QWidget *parent = nullptr);
	void setPerioStatistic(const PerioStatistic& stat, const PerioStatistic* const prev = nullptr);
	~PerioStatisticView();

private:
	Ui::PerioStatisticViewClass ui;
};
