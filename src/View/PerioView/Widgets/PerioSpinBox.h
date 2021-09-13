#pragma once

#include <QSpinBox>

class PerioSpinBox : public QSpinBox
{
	Q_OBJECT

	int m_min{ 0 };
	int m_max{ 0 };
	int m_value{ 0 };

	void paintEvent(QPaintEvent* event) override;

	bool isRed{ false };

public:

	int redValue{ 20 };

	void disable(bool disabled);
	

	PerioSpinBox(QWidget *parent);
	~PerioSpinBox();
};
