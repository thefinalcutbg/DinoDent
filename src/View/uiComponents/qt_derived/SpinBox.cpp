#include "SpinBox.h"

SpinBox::SpinBox(QWidget *parent)
	: QDoubleSpinBox(parent),
	disabled{false}
{
}

SpinBox::~SpinBox()
{
}

void SpinBox::setFocusAndSelectAll()
{
	selectAll();
}

void SpinBox::disable(bool disable)
{
	disabled = disable;
	this->setDisabled(disable);
}

void SpinBox::setAppearence(bool valid)
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
