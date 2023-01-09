#pragma once

#include <QWidget>
#include "ui_StatisticView.h"
#include "View/Interfaces/IStatisticView.h"
#include "Presenter/StatisticPresenter.h"

class StatisticView : public QWidget, public IStatisticView
{
	Q_OBJECT

	StatisticPresenter* presenter{ nullptr };

public:
	StatisticView(QWidget *parent = nullptr);
	void setPresenter(StatisticPresenter* p) override { presenter = p; }
	~StatisticView();

private:
	Ui::StatisticViewClass ui;
};
