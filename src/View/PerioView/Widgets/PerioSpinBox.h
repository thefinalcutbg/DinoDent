#pragma once

#include <QSpinBox>

class PerioSpinBox : public QSpinBox
{
	Q_OBJECT

	int m_min{ 0 };
	int m_max{ 0 };

	bool m_hover{ false };

	void paintEvent(QPaintEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* e) override;

	bool isRed{ false };

public:

	int redValue{ 20 };

	void disable(bool disabled);
	

	PerioSpinBox(QWidget *parent);
	~PerioSpinBox();
};
