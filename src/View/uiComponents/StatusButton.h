#pragma once
#include <QPushButton>
#include "Presenter/CheckState.h"

class StatusButton : public QPushButton
{
	Q_OBJECT

	CheckState checkState;

	void paintEvent(QPaintEvent* e) override;
	bool m_hover{ false };
	bool m_clicked{ false };

	bool eventFilter(QObject* obj, QEvent* e) override;

public:

	StatusButton(QWidget* parent = 0);
	bool pathology{ false };
	void setCheckState(const CheckState& checkState);

};

