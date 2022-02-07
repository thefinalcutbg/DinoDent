#pragma once

#include <QPushButton>

class IconButton : public QPushButton
{
	Q_OBJECT

	bool m_hover{ false };
	bool m_clicked{ false };
	void paintEvent(QPaintEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* e) override;

public:
	IconButton(QWidget *parent);
	~IconButton();
};
