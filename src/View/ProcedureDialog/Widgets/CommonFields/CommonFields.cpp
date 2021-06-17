#include "CommonFields.h"

CommonFields::CommonFields(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.diagnosisEdit, &QTextEdit::textChanged,
		[=] { 
				if(presenter!=nullptr) 
					presenter->diagnosisTextChanged(ui.diagnosisEdit->getText()); 
			});

}

CommonFields::~CommonFields()
{
}

AbstractLineEdit* CommonFields::manipulationEdit()
{
	return ui.manipulationEdit;
}

AbstractLineEdit* CommonFields::diagnosisEdit()
{
	return ui.diagnosisEdit;
}

AbstractSpinBox* CommonFields::priceEdit()
{
	return ui.priceSpinBox;
}

AbstractDateEdit* CommonFields::dateEdit()
{
	return ui.dateEdit;
}

void CommonFields::set_hidden(bool hidden)
{
	setHidden(hidden);
}
