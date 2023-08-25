#pragma once

#include <QPushButton>

class BigPushButton : public QPushButton
{
	Q_OBJECT

		bool m_hover{ false };
	bool m_clicked{ false };
	QColor hoverColor;
	void paintEvent(QPaintEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* e) override;
	QFont m_font;



public:
	BigPushButton(QWidget* parent);
	void setHoverColor(const QColor& color);
	~BigPushButton();
};
