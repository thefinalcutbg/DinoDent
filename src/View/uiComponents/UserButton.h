#pragma once

#include <QToolButton>

class UserButton  : public QToolButton
{
	Q_OBJECT

	bool m_hover{ false };
	bool m_clicked{ false };
	QColor hoverColor;
	void paintEvent(QPaintEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* e) override;
	QFont m_font;
	QSize minimumSizeHint() const override;


public:
	UserButton(QWidget *parent);
	void setHoverColor(const QColor& color);
	~UserButton();
};
