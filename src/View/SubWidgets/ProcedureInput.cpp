#include "ProcedureInput.h"
#include "Presenter/ProcedureCreator.h"

ProcedureInput::ProcedureInput(QWidget *parent)
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

AbstractLineEdit* ProcedureInput::diagnosisEdit()
{
	return ui.diagnosisEdit;
}

AbstractDateEdit* ProcedureInput::dateEdit()
{
	return ui.dateEdit;
}


std::string ProcedureInput::getNotes()
{
	return ui.notesEdit->text().toStdString();
}

QDateEdit* ProcedureInput::qDateEdit()
{
	return ui.dateEdit;
}

AbstractRangeEdit* ProcedureInput::rangeWidget()
{
	return ui.rangeWidget;
}

AbstractSurfaceSelector* ProcedureInput::surfaceSelector()
{
	return ui.surfaceWidget;
}


void ProcedureInput::setErrorMsg(const std::string& error)
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

void ProcedureInput::setLayout(WidgetLayout layout)
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

void ProcedureInput::setNotes(const std::string& notes)
{
	ui.notesEdit->setText(notes.c_str());
}

void ProcedureInput::setNhifLayout(bool nhif)
{
	ui.PHIFcheckbox->setHidden(nhif);
}


void ProcedureInput::setFinancingSource(FinancingSource s)
{
	if (s == FinancingSource::NHIF)
	{
		setNhifLayout(true);
		return;
	}

	ui.PHIFcheckbox->setChecked(s == FinancingSource::PHIF);

}

void ProcedureInput::setHyperdonticState(bool checked)
{
	ui.hyperdonticCheckBox->setChecked(checked);
}

bool ProcedureInput::onHyperdontic()
{
	return !ui.hyperdonticCheckBox->isHidden() && ui.hyperdonticCheckBox->isChecked();
}


FinancingSource ProcedureInput::getFinancingSource()
{
	if (ui.PHIFcheckbox->isHidden()) return FinancingSource::NHIF;

	if (ui.PHIFcheckbox->isChecked()) return FinancingSource::PHIF;

	return FinancingSource::Patient;
}

ProcedureInput::~ProcedureInput()
{
}

