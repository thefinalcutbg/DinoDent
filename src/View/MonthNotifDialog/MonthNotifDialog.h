#pragma once

#include <QDialog>
#include "ui_MonthNotifDialog.h"
#include "MonthNotifTableModel.h"


class MonthNotifDialog : public QDialog
{
	Q_OBJECT


	MonthNotifTableModel model;

	int m_idx{ -1 };



public:
	MonthNotifDialog(const std::vector<MonthNotifRow>& rows, QWidget *parent = Q_NULLPTR);
	int exec() override;
	~MonthNotifDialog();

private:
	Ui::MonthNotifDialog ui;

	//returns the index of the selected row (or -1)

	
};
