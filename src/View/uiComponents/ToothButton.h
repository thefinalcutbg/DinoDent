#pragma once

#include <QPushButton>

enum class LayoutPosition { Left, Center, Right };

class ToothButton : public QPushButton
{
	Q_OBJECT

	bool m_hover{ false };

	LayoutPosition m_layoutPos{ LayoutPosition::Center };

	void paintEvent(QPaintEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* e) override;

public:
	ToothButton(QWidget *parent);

	void setLayoutPos(LayoutPosition p);
	~ToothButton();
};
