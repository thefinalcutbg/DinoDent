#pragma once

#include <QWidget>
#include "ui_ImplantWidget.h"

class ImplantWidget : public QWidget
{
	Q_OBJECT

public:
	ImplantWidget(QWidget *parent = Q_NULLPTR);
	~ImplantWidget();

private:
	Ui::ImplantWidget ui;
};
