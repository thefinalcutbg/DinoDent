#pragma once

#include <QWidget>
#include "ui_PathologyWidget.h"

#include <vector>
#include <string>

#include "Model/Dental/Status.h"

class PathologyWidget : public QWidget
{
	Q_OBJECT

public:
	PathologyWidget(QWidget *parent = Q_NULLPTR);
	void setData(const Pathology& data);
	int getData();
	~PathologyWidget();

private:
	Ui::PathologyWidget ui;
};
