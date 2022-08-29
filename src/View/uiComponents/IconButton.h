#pragma once

#include <QPushButton>

class IconButton : public QPushButton
{
	Q_OBJECT

	bool m_hover{ false };
	bool m_clicked{ false };
	QColor hoverColor;
	QColor backgroundColor;

	void paintEvent(QPaintEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* e) override;

public:
	IconButton(QWidget *parent);
	//sometimes it needs to be changed depending on the background of the parent widget
	void setHoverColor(const QColor& color);
	void setBackgroundColor(const QColor& color);

	~IconButton();
};
