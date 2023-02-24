#include "CommonFields.h"
#include "Presenter/ProcedureCreator.h"

CommonFields::CommonFields(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	


	connect(ui.diagnosisEdit, &QLineEdit::textChanged,
		[=](const QString& text) { 
				if(presenter) 
					presenter->diagnosisTextChanged(text.toStdString()); 
			});


	connect(ui.rangeWidget, &RangeWidget::rangeChanged,
		[=](int begin, int end) {
			//if (presenter)
			//presenter->rangeChanged(begin, end);
		}
	);


	ui.dateEdit->setErrorLabel(ui.errorLabel);
	ui.diagnosisEdit->setErrorLabel(ui.errorLabel);
	ui.rangeWidget->setErrorLabel(ui.errorLabel);
	ui.surfaceWidget->setErrorLabel(ui.errorLabel);
}

AbstractLineEdit* CommonFields::diagnosisEdit()
{
	return ui.diagnosisEdit;
}

AbstractDateEdit* CommonFields::dateEdit()
{
	return ui.dateEdit;
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


void CommonFields::setErrorMsg(const std::string& error)
{
	ui.label_2->setHidden(true);
	ui.diagnosisEdit->setHidden(true);
	ui.rangeWidget->setHidden(true);
	ui.surfaceWidget->setHidden(true);
	ui.notesEdit->setHidden(true);
	ui.PHIFcheckbox->setHidden(true);
	ui.notesLabel->setHidden(true);
	ui.hyperdonticCheckBox->setHidden(true);
	ui.errorLabel->setText(error.c_str());
}

void CommonFields::setLayout(WidgetLayout layout)
{
	ui.label_2->setHidden(false);
	ui.diagnosisEdit->setHidden(false);
	ui.notesEdit->setHidden(false);
	ui.errorLabel->setText("");
	ui.notesLabel->setHidden(false);

	switch (layout)
	{
	case WidgetLayout::General:
		//specific
		ui.rangeWidget->setHidden(true);
		ui.surfaceWidget->setHidden(true);
		ui.hyperdonticCheckBox->setHidden(true);
		break;
	case WidgetLayout::Range:
		ui.rangeWidget->disable(false);
		ui.rangeWidget->setHidden(false);
		ui.surfaceWidget->setHidden(true);
		ui.hyperdonticCheckBox->setHidden(true);
		break;
	case WidgetLayout::Restoration:
		ui.rangeWidget->setHidden(true);
		ui.surfaceWidget->setHidden(false);
		ui.hyperdonticCheckBox->setHidden(false);
		break;
	case WidgetLayout::ToothSpecific:
		ui.rangeWidget->setHidden(true);
		ui.surfaceWidget->setHidden(true);
		ui.hyperdonticCheckBox->setHidden(false);
		break;
	}
}

void CommonFields::setNotes(const std::string& notes)
{
	ui.notesEdit->setText(notes.c_str());
}

void CommonFields::setNhifLayout(bool nhif)
{
	ui.PHIFcheckbox->setHidden(nhif);
}


void CommonFields::setFinancingSource(FinancingSource s)
{
	if (s == FinancingSource::NHIF)
	{
		setNhifLayout(true);
		return;
	}

	ui.PHIFcheckbox->setChecked(s == FinancingSource::PHIF);

}

void CommonFields::setHyperdonticState(bool checked)
{
	ui.hyperdonticCheckBox->setChecked(checked);
}

bool CommonFields::onHyperdontic()
{
	return !ui.hyperdonticCheckBox->isHidden() && ui.hyperdonticCheckBox->isChecked();
}


FinancingSource CommonFields::getFinancingSource()
{
	if (ui.PHIFcheckbox->isHidden()) return FinancingSource::NHIF;

	if (ui.PHIFcheckbox->isChecked()) return FinancingSource::PHIF;

	return FinancingSource::Patient;
}

CommonFields::~CommonFields()
{
}

