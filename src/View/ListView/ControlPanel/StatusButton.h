#pragma once
#include <QPushButton>
#include "Presenter/ListPresenter/CheckState.h"

class StatusButton : public QPushButton
{
	Q_OBJECT

	CheckState checkState;
public:
	StatusButton(QWidget* parent = 0);
	
	void setCheckState(const CheckState& checkState);

};

