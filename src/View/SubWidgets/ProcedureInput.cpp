#include "ProcedureInput.h"
#include "Presenter/ProcedureCreator.h"
#include "QtVersion.h"
#include <QDebug>

ProcedureInput::ProcedureInput(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	for (auto& name : Diagnosis::getNames())
	{
		ui.diagCombo->addItem(name.c_str());
	}

	connect(ui.diagCombo, QtComboIndexChanged,
		[=](int idx) { 

			if (presenter) {
				presenter->diagnosisChanged(idx);
			}

			});


	connect(ui.rangeWidget, &RangeWidget::rangeChanged,
		[=](int begin, int end) {
			//if (presenter)
			//presenter->rangeChanged(begin, end);
		}
	);


	ui.dateEdit->setErrorLabel(ui.errorLabel);
	ui.rangeWidget->setErrorLabel(ui.errorLabel);
	ui.surfaceWidget->setErrorLabel(ui.errorLabel);
}

AbstractLineEdit* ProcedureInput::diagnosisEdit()
{
	return ui.diagEdit;
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

AbstractComboBox* ProcedureInput::diagnosisCombo()
{
	return ui.diagCombo;
}


void ProcedureInput::setErrorMsg(const std::string& error)
{
	ui.addDiagLabel->setHidden(true);
	ui.label_2->setHidden(true);
	ui.diagEdit->setHidden(true);
	ui.diagCombo->setHidden(true);
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
	ui.addDiagLabel->setHidden(false);
	ui.label_2->setHidden(false);
	ui.diagEdit->setHidden(false);
	ui.diagCombo->setHidden(false);
	ui.notesEdit->setHidden(false);
	ui.errorLabel->setText("");
	ui.notesLabel->setHidden(false);

	switch (layout)
	{
		case WidgetLayout::General:
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

