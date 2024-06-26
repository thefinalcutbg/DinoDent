﻿#include "ProcedureInput.h"

#include <QCompleter>
#include <QAbstractItemView>

#include <json/json.h>

#include "Presenter/ProcedureCreator.h"

#include "Resources.h"


ProcedureInput::ProcedureInput(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	for (auto& name : Diagnosis::getNames())
	{
		ui.diagCombo->addItem(name.c_str());
	}

	ui.diagEdit->setInputValidator(&notEmpty_validator);

    connect(ui.diagCombo, &QComboBox::currentIndexChanged, this, [&](int idx) {

			ui.diagEdit->setInputValidator(idx ? nullptr : &notEmpty_validator);

			ui.diagEdit->validateInput();

			if (presenter) {
				presenter->diagnosisChanged(idx);
			}

			});

/*
	connect(ui.rangeWidget, &RangeWidget::rangeChanged,
		[=](int begin, int end) {
			//if (presenter)
			//presenter->rangeChanged(begin, end);
		}
	);
*/

	ui.dateEdit->setErrorLabel(ui.errorLabel);
	ui.rangeWidget->setErrorLabel(ui.errorLabel);
	ui.surfaceWidget->setErrorLabel(ui.errorLabel);
	ui.diagEdit->setErrorLabel(ui.errorLabel);

	//setting autocomplete diagnosis
	Json::Value diagnosisList;

	Json::Reader().parse(Resources::fromPath(":/json/json_diagnosisText.json"), diagnosisList);

	QStringList completionList;
	completionList.reserve(diagnosisList.size());

	for (auto& d : diagnosisList) completionList.append(d.asCString());

	QCompleter* completer = new QCompleter(completionList);

	completer->setCaseSensitivity(Qt::CaseInsensitive);
	completer->setCompletionMode(QCompleter::PopupCompletion);


    QFont f;
	f.setPixelSize(12);
	completer->popup()->setFont(f);
	completer->setMaxVisibleItems(10);
	completer->setModelSorting(QCompleter::UnsortedModel);
	ui.diagEdit->setCompleter(completer);

	
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
	return ui.notesEdit->toPlainText().toStdString();
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

int ProcedureInput::minutes()
{
	return ui.anesthesiaSpin->value();
}

void ProcedureInput::setMinutes(int min)
{
	ui.anesthesiaSpin->setValue(min);
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
	ui.anesthesiaGroup->setHidden(true);
	ui.errorLabel->setText(error.c_str());
	ui.dateEdit->setErrorLabel(nullptr);
	ui.rangeWidget->setErrorLabel(nullptr);
	ui.surfaceWidget->setErrorLabel(nullptr);
	ui.diagEdit->setErrorLabel(nullptr);

}


void ProcedureInput::setLayout(WidgetLayout layout)
{
	ui.addDiagLabel->setHidden(false);
	ui.label_2->setHidden(false);
	ui.diagEdit->setHidden(false);
	ui.diagCombo->setHidden(false);
	ui.notesEdit->setHidden(false);

	ui.dateEdit->setErrorLabel(ui.errorLabel);
	ui.rangeWidget->setErrorLabel(ui.errorLabel);
	ui.surfaceWidget->setErrorLabel(ui.errorLabel);
	ui.diagEdit->setErrorLabel(ui.errorLabel);

	ui.errorLabel->setText("");
	ui.notesLabel->setHidden(false);

	switch (layout)
	{
		case WidgetLayout::General:
			ui.rangeWidget->setHidden(true);
			ui.surfaceWidget->setHidden(true);
			ui.hyperdonticCheckBox->setHidden(true);
			ui.anesthesiaGroup->setHidden(true);
			break;
		case WidgetLayout::Range:
			ui.rangeWidget->disable(false);
			ui.rangeWidget->setHidden(false);
			ui.surfaceWidget->setHidden(true);
			ui.hyperdonticCheckBox->setHidden(true);
			ui.anesthesiaGroup->setHidden(true);
			break;
		case WidgetLayout::Restoration:
			ui.rangeWidget->setHidden(true);
			ui.surfaceWidget->setHidden(false);
			ui.hyperdonticCheckBox->setHidden(false);
			ui.anesthesiaGroup->setHidden(true);
			break;
		case WidgetLayout::ToothSpecific:
			ui.rangeWidget->setHidden(true);
			ui.surfaceWidget->setHidden(true);
			ui.hyperdonticCheckBox->setHidden(false);
			ui.anesthesiaGroup->setHidden(true);
			break;
		case WidgetLayout::Anesthesia:
			ui.rangeWidget->setHidden(true);
			ui.surfaceWidget->setHidden(true);
			ui.hyperdonticCheckBox->setHidden(true);
			ui.anesthesiaGroup->setHidden(false);
	}
}

void ProcedureInput::setNotes(const std::string& notes)
{
	ui.notesEdit->setPlainText(notes.c_str());
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

