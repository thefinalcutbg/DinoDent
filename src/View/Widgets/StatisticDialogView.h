#pragma once

#include <QDialog>
#include "ui_StatisticDialogView.h"
#include "View/Interfaces/IStatisticDialogView.h"

class StatisticDialogPresenter;

class StatisticDialogView : public QDialog, public IStatisticDialogView
{
	Q_OBJECT

	StatisticDialogPresenter& presenter;

public:
	StatisticDialogView(StatisticDialogPresenter& p, QWidget *parent = nullptr);
	~StatisticDialogView();

private:
	Ui::StatisticDialogViewClass ui;

	// Inherited via IStatisticDialogView
	void setNameFilterList(const std::vector<std::string> namesFilter) override;
	void setDiagnosisFilterList(const std::vector<std::string> diagnosis) override;
	void setStatistic(const DentalStatistic& s) override;
	DentalStatistic getStatistic() override;
	void closeDialog() override { close(); }
	void hideToothFilter(bool hidden);
};