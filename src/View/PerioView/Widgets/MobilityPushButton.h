#pragma once

#include "PerioButton.h"

class MobilityPushButton : public PerioButton
{
	Q_OBJECT

	short int m_mobility{ 0 };

	//void paintEvent(QPaintEvent* event) override;

public:

	MobilityPushButton(QWidget *parent);

	void setMobility(int mobility);

	~MobilityPushButton();
};
