#pragma once

#include <QWidget>
#include "ui_PerioSurfaceHeader.h"
#include "MobilityPushButton.h"

class PerioSurfaceHeader : public QWidget
{
	Q_OBJECT

	QPushButton* m_tooth[16];
	MobilityPushButton* m_mobi[16];
	PerioButton* m_implant[16];


	void toothStateSlot(int tooth, bool present);

public:
	PerioSurfaceHeader(QWidget *parent = Q_NULLPTR);
	void fixSize();
	~PerioSurfaceHeader();
signals:
	emit void toothStateChanged(int tooth, bool present);

private:
	Ui::PerioSurfaceHeader ui;
};
