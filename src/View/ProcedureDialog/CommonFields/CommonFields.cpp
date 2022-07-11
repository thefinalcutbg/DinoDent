#include "CommonFields.h"
#include "Presenter/ProcedureDialog/SubPresenters/AbstractSubPresenter.h"

CommonFields::CommonFields(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.diagnosisEdit, &QTextEdit::textChanged,
		[=] { 
				if(presenter) 
					presenter->diagnosisTextChanged(ui.diagnosisEdit->getText()); 
			});

	connect(ui.ksmpButton, &QPushButton::clicked, 
		[=] {if (presenter)presenter->ksmpButtonClicked();}
	);
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

QPushButton* CommonFields::ksmpButton()
{
	return ui.ksmpButton;
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

void CommonFields::setKSMPButtonCode(const std::string& code)
{
	ui.ksmpButton->setText(code.c_str());
}

void CommonFields::set_hidden(bool hidden)
{
	setHidden(hidden);
}
