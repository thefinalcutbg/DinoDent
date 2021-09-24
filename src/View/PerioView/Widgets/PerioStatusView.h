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

	PerioScene* perioScene;

	PerioChartItem* upperChart;
	PerioChartItem* lowerChart;

	QPushButton* m_tooth[16];
	PerioStateButton* m_mobi[16];
	FurcationWidget* m_furcation[16];
	PerioSpinBox* m_Rec[32];
	PerioSpinBox* m_Attach[32];
	PerioSpinBox* m_PD[96];
	PerioSpinBox* m_CAL[96];
	PerioSpinBox* m_GM[96];
	PerioButton* m_BOP[96];

	void calcRecession(int GM_idx);
	void disableColumn(int toothIdx, bool disabled);


public:


	PerioStatusView(QWidget *parent = Q_NULLPTR);

	void setTooth(const ToothPaintHint& hint);
	void setPerioStatus(const PerioStatus& status);
	PerioStatus getPerioStatus();
	~PerioStatusView();

private:
	Ui::PerioStatusView ui;
};
