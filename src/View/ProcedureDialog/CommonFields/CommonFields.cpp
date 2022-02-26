#include "CommonFields.h"
#include "Presenter/ProcedureDialog/SubPresenters/AbstractSubPresenter.h"

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

void CommonFields::setExternalDateEdit(DateEdit* externalDateEdit)
{
	this->externalDateEdit = externalDateEdit;
	ui.dateEdit->setHidden(true);
	ui.dateLabel->setHidden(true);
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
	if (externalDateEdit == nullptr){
		return ui.dateEdit;
	}
	
	return externalDateEdit;
}

void CommonFields::set_hidden(bool hidden)
{
	setHidden(hidden);
}
