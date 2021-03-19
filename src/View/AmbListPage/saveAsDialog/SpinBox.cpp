#include "SpinBox.h"


SpinBox::SpinBox(QWidget* parent) : QSpinBox(parent), validNumber(NULL), msgLabel(NULL), validState(true)
{
	connect(this, QOverload<int>::of(&QSpinBox::valueChanged), this, [=] {validate(); });
}

bool SpinBox::isValid()
{
	return validState;
}

void SpinBox::validate()
{
	if (validNumber == NULL || msgLabel == NULL) return;

	validState = !(*validNumber)[value()];

	if (validState)
	{
		setStyleSheet("");
		msgLabel->setText("");
	}
	else
	{
		setStyleSheet("border: 1px solid red;");
		msgLabel->setText("Амбулаторен лист с такъв номер вече съществува!");
	}
	if (value() == 0) msgLabel->setText("Номерът на амбулаторният лист не може да е с нулева стойност");
}

void SpinBox::setMap(std::map<int, bool>* existingNumbersMap)
{
	this->validNumber = existingNumbersMap;
}

void SpinBox::setLabelMsg(QLabel* label)
{
	msgLabel = label;
}