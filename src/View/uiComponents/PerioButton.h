#pragma once

#include <QPushButton>

class PerioButton : public QAbstractButton
{
	Q_OBJECT

	bool m_hover{ false };

protected:

	void paintEvent(QPaintEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* e) override;
	

public:
	PerioButton(QWidget *parent);

	virtual ~PerioButton();
};
