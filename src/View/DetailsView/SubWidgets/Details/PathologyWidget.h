#pragma once

#include <QWidget>
#include "ui_PathologyWidget.h"

class PathologyWidget : public QWidget
{
	Q_OBJECT

public:
	PathologyWidget(QWidget *parent = Q_NULLPTR);
	~PathologyWidget();

private:
	Ui::PathologyWidget ui;
};
