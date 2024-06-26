#include "SpinBox.h"

SpinBox::SpinBox(QWidget *parent)
	: QDoubleSpinBox(parent),
	disabled{false}
{
}

SpinBox::~SpinBox()
{
}

void SpinBox::set_focus()
{
	selectAll();
}

void SpinBox::disable(bool disable)
{
	disabled = disable;
	this->setDisabled(disable);
}

void SpinBox::hide(bool hidden)
{
	setHidden(hidden);
}

void SpinBox::setValidAppearence(bool)
{
}

void SpinBox::set_Value(double value)
{
	setValue(value);
}

double SpinBox::get_Value()
{
	return value();
}
