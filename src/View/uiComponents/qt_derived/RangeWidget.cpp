#include "RangeWidget.h"

RangeWidget::RangeWidget(QWidget *parent)
	: QWidget(parent),
		validator(nullptr)
{
	ui.setupUi(this);

	ui.errorLabel->setStyleSheet("color:red");

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

	connect(ui.checkBox, &QCheckBox::stateChanged, 
		[=] 
		{ 
				bool disable = !ui.checkBox->isChecked(); 

				ui.label_3->setHidden(disable);
				ui.label_4->setHidden(disable);
				ui.endCombo->setHidden(disable);
				ui.beginCombo->setHidden(disable);
				ui.errorLabel->setHidden(disable);

				emit widgetChecked(!disable);
		});

	ui.label_4->setHidden(true);
	ui.label_3->setHidden(true);
	ui.endCombo->setHidden(true);
	ui.beginCombo->setHidden(true);
}

RangeWidget::~RangeWidget()
{


}

void RangeWidget::disable(bool disable)
{
	ui.checkBox->setChecked(!disable);
}


void RangeWidget::setRange(int begin, int end)
{
	ui.beginCombo->blockSignals(true);
	ui.endCombo->blockSignals(true);
	ui.beginCombo->setCurrentIndex(begin);
	ui.endCombo->setCurrentIndex(end);
	ui.beginCombo->blockSignals(false);
	ui.endCombo->blockSignals(false);

	AbstractUIElement::validateInput();
}

std::tuple<int, int> RangeWidget::getRange()
{
	int begin = ui.beginCombo->currentIndex();
	int end = ui.endCombo->currentIndex();

		if (begin > end) {
		std::swap(begin, end);
	}

	return std::tuple<int, int>(begin, end);
}

void RangeWidget::setValidAppearence(bool valid)
{
	if (valid) {

		ui.errorLabel->setText("");
	}
	else
	{
		ui.errorLabel->setText("Невалидна дължина на конструкцията");
	}
}

void RangeWidget::setFocus()
{
	ui.endCombo->setFocus();
	ui.endCombo->showPopup();
}

void RangeWidget::disbleBridgeSwitch(bool disable)
{
	ui.checkBox->setDisabled(disable);
//	ui.beginCombo->setDisabled(disable);
//	ui.endCombo->setDisabled(disable);
}


