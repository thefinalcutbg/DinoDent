#pragma once

#include <QWidget>
#include "ui_ObturationWidget.h"

class ObturationWidget : public QWidget
{
	Q_OBJECT

public:
	ObturationWidget(QWidget *parent = Q_NULLPTR);
	~ObturationWidget();

private:
	Ui::ObturationWidget ui;
};
