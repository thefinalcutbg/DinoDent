#pragma once

#include <QDialog>
#include "ui_CurrentStatusDialog.h"

class ToothContainer;

class CurrentStatusDialog : public QDialog
{
	Q_OBJECT

	void paintEvent(QPaintEvent* e) override;


public:
	CurrentStatusDialog(const ToothContainer& t, QWidget *parent = nullptr);
	~CurrentStatusDialog();

private:
	Ui::CurrentStatusDialogClass ui;
};
