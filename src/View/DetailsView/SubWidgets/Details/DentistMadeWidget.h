#pragma once

#include <QWidget>
#include "ui_DentistMadeWidget.h"

class DentistMadeWidget : public QWidget
{
	Q_OBJECT

public:
	DentistMadeWidget(QWidget *parent = Q_NULLPTR);
	~DentistMadeWidget();

private:
	Ui::DentistMadeWidget ui;
};
