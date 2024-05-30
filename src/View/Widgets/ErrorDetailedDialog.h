#pragma once

#include <QDialog>
#include "ui_ErrorDetailedDialog.h"
#include <string>

class ErrorDetailedDialog : public QDialog
{
	Q_OBJECT

public:
	ErrorDetailedDialog(const std::string& description, const std::string& details);
	~ErrorDetailedDialog();

private:
	Ui::ErrorDetailedDialogClass ui;
};
