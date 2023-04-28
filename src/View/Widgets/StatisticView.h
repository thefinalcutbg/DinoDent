#pragma once

#include <QWidget>
#include "ui_StatisticView.h"
#include "View/Interfaces/IStatisticView.h"
#include "Presenter/StatisticPresenter.h"
#include "View/TableModels/StatisticTableModel.h"

class StatisticView : public QWidget, public IStatisticView
{
	Q_OBJECT

	StatisticPresenter* presenter{ nullptr };
	StatisticTableModel m_model;
	void dateChanged();
public:
	StatisticView(QWidget *parent = nullptr);
	void setPresenter(StatisticPresenter* p) override { presenter = p; }
	void setStatList(const std::vector<std::pair<std::string, int>>& stat) override;
	void setYear(int year) override;
	~StatisticView();

private:
	Ui::StatisticViewClass ui;

};
