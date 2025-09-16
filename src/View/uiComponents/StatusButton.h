#pragma once
#include <QPushButton>
#include "Presenter/CheckModel.h"

class StatusButton : public QPushButton
{
	Q_OBJECT

	CheckState checkState;

	void paintEvent(QPaintEvent* e) override;
	bool m_hover{ false };
	bool m_clicked{ false };

	bool eventFilter(QObject* obj, QEvent* e) override;

	void mousePressEvent(QMouseEvent* e) override;

public:

	StatusButton(QWidget* parent = 0);
	bool pathology{ false };
	void setCheckState(const CheckState& checkState);

signals:
	void rightClicked();
};

