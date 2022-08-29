#pragma once

#include <QDialog>
#include "ui_UpdateDialog.h"

class UpdateDialog : public QDialog
{
	Q_OBJECT

public:
	UpdateDialog(QWidget *parent = nullptr);
	void setRange(int range);
	void increment();
	~UpdateDialog();

private:
	Ui::UpdateDialogClass ui;
};
