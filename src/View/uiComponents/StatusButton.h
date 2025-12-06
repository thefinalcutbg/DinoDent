#pragma once
#include <QPushButton>
#include "Presenter/CheckModel.h"

class QVariantAnimation;

class StatusButton : public QPushButton
{
	Q_OBJECT

		CheckState checkState;
	bool m_hover{ false };
	bool m_clicked{ false };

	qreal m_checkProgress{ 0.0 };
	QVariantAnimation* m_checkAnimation{ nullptr };

	qreal m_hoverProgress{ 0.0 };
	QVariantAnimation* m_hoverAnimation{ nullptr };

	void paintEvent(QPaintEvent* e) override;
	bool eventFilter(QObject* obj, QEvent* e) override;
	void mousePressEvent(QMouseEvent* e) override;

public:
	StatusButton(QWidget* parent = 0);
	bool pathology{ false };
	void setCheckState(const CheckState& checkState);

signals:
	void rightClicked();
};
