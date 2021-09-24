#pragma once

#include <QPushButton>

class PerioButton : public QAbstractButton
{
	Q_OBJECT


protected:

	void paintEvent(QPaintEvent* event) override;

public:
	PerioButton(QWidget *parent);

	~PerioButton();
};
