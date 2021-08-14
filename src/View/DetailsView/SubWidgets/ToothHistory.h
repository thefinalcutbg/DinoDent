#pragma once

#include <QWidget>
#include "ui_ToothHistory.h"

class ToothHistory : public QWidget
{
	Q_OBJECT

public:
	ToothHistory(QWidget *parent = Q_NULLPTR);
	~ToothHistory();

private:
	Ui::ToothHistory ui;
};
