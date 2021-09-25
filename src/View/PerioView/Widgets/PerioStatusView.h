#pragma once

#include <QWidget>

#include "ui_PerioStatusView.h"

#include "PerioStateButton.h"
#include "FurcationWidget.h"
#include "PerioSpinBox.h"

class PerioScene;
class PerioChartItem;
class ToothPaintHint;
class PerioStatus;

class PerioStatusView : public QWidget
{
	Q_OBJECT

public:


	PerioStatusView(QWidget *parent = Q_NULLPTR);

	~PerioStatusView();

	Ui::PerioStatusView ui;
	
};
