#pragma once

#include <QDialog>
#include "ui_StatisticDialogView.h"

#include "Model/Dental/DentalStatistics.h"
#include "Model/Dental/ProcedureCode.h"

class StatisticDialogPresenter;

class StatisticDialogView : public QDialog
{
	Q_OBJECT

	StatisticDialogPresenter& presenter;
	std::vector<std::string> m_procedureCodes;

	void paintEvent(QPaintEvent* e) override;

public:
	StatisticDialogView(StatisticDialogPresenter& p, QWidget *parent = nullptr);

	void setProcedureFilter(const std::vector<ProcedureCode> namesFilter);
	DentalStatistic getStatistic();

	void hideToothFilter(bool hidden);
	void setResult(int count);

	~StatisticDialogView();

private:
	Ui::StatisticDialogViewClass ui;


};
