#pragma once

#include <QWidget>
#include "ui_PerioSurfaceData.h"
#include "PerioSpinBox.h"
#include <QPushButton>
#include "PerioButton.h"
#include "Measurment.h"

class PerioSurfaceData : public QWidget
{
	Q_OBJECT

	PerioSpinBox* m_PD[48];
	PerioSpinBox* m_CAL[48];
	PerioSpinBox* m_GM[48];
	PerioButton* m_BOP[48];
	PerioSpinBox* m_Rec[16];
	PerioSpinBox* m_Attach[16];

	void calcRecession(int surface);

public:
	PerioSurfaceData(QWidget *parent = Q_NULLPTR);
	~PerioSurfaceData();

	void setExtracted(int tooth, bool extracted);
	void setLingual(bool lingual);

signals:
	void measuremetChanged(Measurment m);

private:
	Ui::PerioSurfaceData ui;
};
