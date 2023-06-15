#pragma once

#include <QDialog>
#include "ui_AddTaxDialog.h"

class AddTaxDialog : public QDialog
{
	Q_OBJECT

public:
	AddTaxDialog(QWidget *parent = nullptr);
	~AddTaxDialog();

private:
	Ui::AddTaxDialogClass ui;
};
