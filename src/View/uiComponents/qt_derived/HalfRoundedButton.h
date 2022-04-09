#pragma once

#include <QPushButton>

class HalfRoundedButton : public QPushButton
{
	Q_OBJECT

	bool m_hover{ false };
	bool m_reversed{ false };
	void paintEvent(QPaintEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* e) override;

public:
	HalfRoundedButton(QWidget *parent);
	void setReversed() { m_reversed = true; };
	~HalfRoundedButton();
};
