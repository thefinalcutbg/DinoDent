#pragma once

#include "ui_MonthNotifView.h"

#include "View/TableModels/MonthNotifTableModel.h"

class MonthNotifPresenter;

class MonthNotifView final: public QWidget
{
	Q_OBJECT

	MonthNotifPresenter* presenter{nullptr};

	MonthNotifTableModel model;

	QDialog* parent_dialog{nullptr};

	int m_idx{ -1 };

	bool first_show{ true };

public:
	MonthNotifView(QWidget *parent = nullptr);
	void setPresenter(MonthNotifPresenter* presenter);
	void hasBeenShown();
	void setParentDialog(QDialog* d) { parent_dialog = d; }

	void setMonthNotifRows(const std::vector<MonthNotifRow> rows);
	void closeParentDialog();

	~MonthNotifView();

private:
	Ui::MonthNotifViewClass ui;

};
