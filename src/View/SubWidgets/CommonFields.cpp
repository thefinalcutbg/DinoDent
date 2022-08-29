#include "CommonFields.h"
#include "Presenter/AbstractSubPresenter.h"

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
		[=] {	if (presenter)presenter->ksmpButtonClicked();}
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


AbstractLineEdit* CommonFields::procedureNameEdit()
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

void CommonFields::setKSMPCode(const std::string& code)
{
	ui.ksmpButton->setText(code.c_str());
}

void CommonFields::enableKSMP(bool enabled)
{
	ui.ksmpButton->setEnabled(enabled);
}

std::string CommonFields::getKSMPCode()
{
	return ui.ksmpButton->text().toStdString();
}

void CommonFields::set_hidden(bool hidden)
{
	setHidden(hidden);
}
