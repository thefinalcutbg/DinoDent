#pragma once

#include <QWidget>
#include "ui_PathologyWidget.h"

#include <vector>
#include <string>

#include "Model/Dental/StatusData.h"

class PathologyWidget : public QWidget
{
	Q_OBJECT

public:
	PathologyWidget(QWidget *parent = Q_NULLPTR);
	void setData(const PathologyData& data);
	int getData();
	~PathologyWidget();

private:
	Ui::PathologyWidget ui;
};
