#include "MnogoTypSpinBox.h"


MnogoTypSpinBox::MnogoTypSpinBox(QWidget* parent) : QSpinBox(parent), validNumber(nullptr), msgLabel(nullptr), validState(true)
{
	connect(this, QOverload<int>::of(&QSpinBox::valueChanged), this, [=] {validate(); });
}

bool MnogoTypSpinBox::isValid()
{
	return validState;
}

void MnogoTypSpinBox::validate()
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

void MnogoTypSpinBox::setMap(std::map<int, bool>* existingNumbersMap)
{
	this->validNumber = existingNumbersMap;
}

void MnogoTypSpinBox::setLabelMsg(QLabel* label)
{
	msgLabel = label;
}