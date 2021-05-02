#include "StatusButton.h"

StatusButton::StatusButton(QWidget* parent) : QPushButton(parent), checkState(CheckState::unchecked)
{
	setCheckable(true);
	setFocusPolicy(Qt::NoFocus);
}

void StatusButton::setCheckState(const CheckState& checkState)
{
	this->checkState = checkState;

	if (checkState == CheckState::checked) {
		setChecked(1);
		return;
	}

	setChecked(0);
}
