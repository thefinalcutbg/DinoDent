#pragma once

#include <QPushButton>

class PerioButton : public QAbstractButton
{
	Q_OBJECT


protected:

	QString m_text; //custom text, which does not change the button width in layout

	void paintEvent(QPaintEvent* event) override;

public:
	PerioButton(QWidget *parent);

	~PerioButton();
};
