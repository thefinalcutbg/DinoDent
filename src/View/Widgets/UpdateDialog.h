#pragma once

#include <QDialog>
#include "ui_UpdateDialog.h"
//if the update operation takes longer, showing this dialog is recommended
class UpdateDialog : public QDialog
{
	Q_OBJECT

public:
	UpdateDialog(QWidget *parent = nullptr);
	void execute();
	void setRange(int range);
	void increment();
	~UpdateDialog();

private:
	Ui::UpdateDialogClass ui;
};
