#include "CommonFields.h"
#include "Presenter/AbstractSubPresenter.h"

CommonFields::CommonFields(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	


	connect(ui.diagnosisEdit, &QLineEdit::textChanged,
		[=](const QString& text) { 
				if(presenter) 
					presenter->diagnosisTextChanged(text.toStdString()); 
			});

	connect(ui.ksmpButton, &QPushButton::clicked, 
		[=] {	if (presenter)presenter->ksmpButtonClicked();}
	);

	connect(ui.rangeWidget, &RangeWidget::rangeChanged,
		[=](int begin, int end) {
			if (presenter)
			presenter->rangeChanged(begin, end);
		}
	);

	connect(ui.bridgeCheckBox, &QCheckBox::toggled,
		[=](bool checked) {

			if (presenter)
			presenter->bridgeChecked(ui.bridgeCheckBox->isChecked());
		}
	);

	ui.dateEdit->setErrorLabel(ui.errorLabel);
	ui.diagnosisEdit->setErrorLabel(ui.errorLabel);
	ui.procedureEdit->setErrorLabel(ui.errorLabel);
	ui.rangeWidget->setErrorLabel(ui.errorLabel);
	ui.surfaceWidget->setErrorLabel(ui.errorLabel);
}

AbstractLineEdit* CommonFields::procedureNameEdit()
{
	return ui.procedureEdit;
}

AbstractLineEdit* CommonFields::diagnosisEdit()
{
	return ui.diagnosisEdit;
}

AbstractDateEdit* CommonFields::dateEdit()
{
	return ui.dateEdit;
}

void CommonFields::setKSMPCode(const std::string& code)
{
	ui.ksmpButton->setText(code.c_str());
}

std::string CommonFields::getKSMPCode()
{
	return ui.ksmpButton->text().toStdString();
}

std::string CommonFields::getNotes()
{
	return ui.notesEdit->text().toStdString();
}

QDateEdit* CommonFields::qDateEdit()
{
	return ui.dateEdit;
}

AbstractRangeEdit* CommonFields::rangeWidget()
{
	return ui.rangeWidget;
}

AbstractSurfaceSelector* CommonFields::surfaceSelector()
{
	return ui.surfaceWidget;
}

void CommonFields::setBridgeCheckState(BridgeCheckState state)
{
	QSignalBlocker b(ui.bridgeCheckBox);

	switch (state)
	{
	case BridgeCheckState::Hidden:
		ui.bridgeCheckBox->hide(); 
		break;
	case BridgeCheckState::Checked:
		ui.bridgeCheckBox->show();
		ui.bridgeCheckBox->setChecked(true);
		break;
	case BridgeCheckState::Unchecked:
		ui.bridgeCheckBox->show();
		ui.bridgeCheckBox->setChecked(false);
		break;

	}
}

void CommonFields::disableBridgeCheck(bool disabled)
{
	ui.bridgeCheckBox->setDisabled(disabled);
}

void CommonFields::setErrorMsg(const std::string& error)
{
	ui.label_2->setHidden(true);
	ui.bridgeCheckBox->setHidden(true);
	ui.diagnosisEdit->setHidden(true);
	ui.ksmpButton->setHidden(true);
	ui.label_3->setHidden(true);
	ui.procedureEdit->setHidden(true);
	ui.rangeWidget->setHidden(true);
	ui.surfaceWidget->setHidden(true);
	ui.notesEdit->setHidden(true);
	ui.PHIFcheckbox->setHidden(true);
	ui.notesLabel->setHidden(true);
	
	ui.errorLabel->setText(error.c_str());
}

void CommonFields::setLayout(WidgetLayout layout)
{
	ui.label_2->setHidden(false);
	ui.diagnosisEdit->setHidden(false);
	ui.ksmpButton->setHidden(false);
	ui.label_3->setHidden(false);
	ui.procedureEdit->setHidden(false);
	ui.notesEdit->setHidden(false);
	ui.errorLabel->setText("");
	ui.notesLabel->setHidden(false);

	switch (layout)
	{
	case WidgetLayout::General:

		//specific
		ui.rangeWidget->setHidden(true);
		ui.surfaceWidget->setHidden(true);
		ui.bridgeCheckBox->setHidden(true);
		break;
	case WidgetLayout::Restoration:
		ui.rangeWidget->setHidden(true);
		ui.surfaceWidget->setHidden(false);
		ui.bridgeCheckBox->setHidden(true);
		break;
	case WidgetLayout::Crown:
		ui.rangeWidget->setHidden(false);
		ui.surfaceWidget->setHidden(true);
		ui.bridgeCheckBox->setHidden(false);
		break;
	case WidgetLayout::Range:
		ui.rangeWidget->disable(false);
		ui.rangeWidget->setHidden(false);
		ui.surfaceWidget->setHidden(true);
		ui.bridgeCheckBox->setHidden(true);
		break;
	}
}

void CommonFields::setNotes(const std::string& notes)
{
	ui.notesEdit->setText(notes.c_str());
}

void CommonFields::setNhifLayout(bool nhif)
{
	ui.ksmpButton->setDisabled(nhif);
	ui.PHIFcheckbox->setHidden(nhif);
	ui.procedureEdit->disable(nhif);
}


void CommonFields::setFinancingSource(Procedure::FinancingSource s)
{
	if (s == Procedure::NHIF)
	{
		setNhifLayout(true);
		return;
	}

	ui.PHIFcheckbox->setChecked(s == Procedure::PHIF);

}


Procedure::FinancingSource CommonFields::getFinancingSource()
{
	if (ui.PHIFcheckbox->isHidden()) return Procedure::NHIF;

	if (ui.PHIFcheckbox->isChecked()) return Procedure::PHIF;

	return Procedure::Patient;
}

CommonFields::~CommonFields()
{
}

