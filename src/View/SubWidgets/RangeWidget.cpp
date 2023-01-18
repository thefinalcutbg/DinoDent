#include "RangeWidget.h"

RangeWidget::RangeWidget(QWidget *parent)
	: QWidget(parent),
		validator(nullptr),
		errorLabel(nullptr)
{
	ui.setupUi(this);

	QString toothIndexes[32]{ "18", "17", "16", "15", "14", "13", "12", "11",
						  "21", "22", "23", "24", "25", "26", "27", "28",
						  "38", "37", "36", "35", "34", "33", "32", "31",
						  "41", "42", "43", "44", "45", "46", "47", "48" };

	for (int i = 0; i < 32; i++)
	{
		ui.beginCombo->addItem(toothIndexes[i]);
		ui.endCombo->addItem(toothIndexes[i]);
	}

	connect(ui.beginCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index)
		{ 	
			AbstractUIElement::validateInput();

			if (isValid())
			{
				auto [begin, end] = getRange();
				emit rangeChanged(begin, end);
			} 
		});

	connect(ui.endCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[=](int index)
		{
			AbstractUIElement::validateInput();

			if (isValid())
			{
				auto [begin, end] = getRange();
				emit rangeChanged(begin, end);
			}
		});
		
	setErrorLabel(ui.errorLabel);
	
}

RangeWidget::~RangeWidget()
{


}


void RangeWidget::disable(bool disable)
{
	setDisabled(disable);
}

void RangeWidget::setBridgeRange(int begin, int end)
{
	QSignalBlocker b(ui.beginCombo);
	QSignalBlocker e(ui.endCombo);

	ui.beginCombo->setCurrentIndex(begin);
	ui.endCombo->setCurrentIndex(end);

	AbstractUIElement::validateInput();
}

std::tuple<int, int> RangeWidget::getRange()
{
	int begin = ui.beginCombo->currentIndex();
	int end = ui.endCombo->currentIndex();

	if (begin > end) std::swap(begin, end);

	return { begin, end };
}

void RangeWidget::setValidAppearence(bool valid)
{
	if (errorLabel == nullptr) return;

	if (valid) {

		errorLabel->setText("");
	}
	else
	{
		errorLabel->setText("Невалидна дължина на конструкцията");
	}
}

void RangeWidget::setFocus()
{
	ui.endCombo->setFocus();
	ui.endCombo->showPopup();
}

void RangeWidget::setErrorLabel(QLabel* errorLabel)
{
	this->errorLabel = errorLabel;
}


