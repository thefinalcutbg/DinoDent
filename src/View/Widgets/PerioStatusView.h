#pragma once

#include <QWidget>

#include "ui_PerioStatusView.h"

#include "View/uiComponents/PerioStateButton.h"
#include "View/uiComponents/FurcationWidget.h"
#include "View/uiComponents/PerioSpinBox.h"

class PerioScene;
class PerioChartItem;
struct ToothPaintHint;
class PerioStatus;

class PerioStatusView : public QWidget
{
	Q_OBJECT

public:


	PerioStatusView(QWidget *parent = Q_NULLPTR);

	~PerioStatusView();

	Ui::PerioStatusView ui;
	
};
