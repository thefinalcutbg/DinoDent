#pragma once

#include <QDialog>
#include "ui_ReportErrorDialog.h"
#include <string>

class ReportErrorDialog : public QDialog
{
	Q_OBJECT

public:
	ReportErrorDialog(const std::string& errors, QWidget *parent = Q_NULLPTR);
	~ReportErrorDialog();

private:
	Ui::ReportErrorDialog ui;
};
