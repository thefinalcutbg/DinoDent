#pragma once

#include <QPushButton>

class QVariantAnimation;

class HoverButton : public QPushButton
{
	Q_OBJECT

	QString m_hoverText;
	bool m_hover{ false };
	qreal m_hoverProgress{ 0.0 };
	QVariantAnimation* m_hoverAnimation{ nullptr };

	void paintEvent(QPaintEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* e) override;

public:
	HoverButton(QWidget* parent);
	void setHoverText(const QString& text) { m_hoverText = text; update(); };

	~HoverButton();
};
