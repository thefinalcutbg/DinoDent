#pragma once

#include <QDialog>
#include "ui_PisDialog.h"

class PisDialog : public QDialog
{
	Q_OBJECT

public:
	PisDialog(QWidget *parent = nullptr);
	MonthNotifView* monthNotifView() { return ui.monthNotifView; }
	~PisDialog();

private:
	Ui::PisDialogClass ui;
};
