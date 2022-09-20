#pragma once

#include "ui_MonthNotifView.h"
#include "View/Interfaces/IMonthNotifView.h"
#include "View/Models/MonthNotifTableModel.h"

class MonthNotifPresenter;

class MonthNotifView final: public QWidget, public IMonthNotifView
{
	Q_OBJECT

	MonthNotifPresenter* presenter{nullptr};

	MonthNotifTableModel model;

	QDialog* parent_dialog{nullptr};

	int m_idx{ -1 };

	void paintEvent(QPaintEvent* event) override;

public:
	MonthNotifView(QWidget *parent = nullptr);
	void setPresenter(MonthNotifPresenter* presenter);

	// Inherited via IMonthNotifView
	void setMonthNotifRows(const std::vector<MonthNotifRow> rows) override;
	void closeParentDialog() override;
	void setParentDialog(QDialog* d) { parent_dialog = d; }
	~MonthNotifView();

private:
	Ui::MonthNotifViewClass ui;

};
