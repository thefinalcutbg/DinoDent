#include "ProcedureInput.h"

#include <QCompleter>
#include <QAbstractItemView>

#include <json/json.h>
#include "View/Widgets/TableViewDialog.h"
#include "Presenter/ProcedureCreator.h"
#include "Resources.h"
#include "View/ModalDialogBuilder.h"
#include "View/Widgets/ICD10Dialog.h"

ProcedureInput::ProcedureInput(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlag(Qt::WindowMaximizeButtonHint);

	connect(ui.rangeCheck, &QCheckBox::toggled, this, [&](bool checked) {
		ui.rangeGroup->setHidden(!checked);
		ui.hyperdonticCheckBox->setHidden(checked);
		});

	connect(ui.constructionTypeCombo, &QComboBox::currentIndexChanged, this, [&](int index) {

		switch (index)
		{	
			case 0: setParameterData(ui.hyperdonticCheckBox->isChecked()); break; //crown
			case 1: setParameterData(getConstructionRange(), false); break; //bridge
			case 2: setParameterData(ui.hyperdonticCheckBox->isChecked(), getRestorationData()); break; //restoration
		}

		ui.constructionTypeGroup->show();
	});

	connect(ui.icdButton, &QPushButton::clicked, this, [&] {

			auto result = ModalDialogBuilder::icdDialog(ui.icdButton->text().toStdString());

			if (result.isValid()) {
				ui.icdEdit->setText(result.name().c_str());
				ui.icdEdit->setCursorPosition(0);
				ui.icdButton->setText(result.code().c_str());
			}
	});

	connect(ui.icdEdit, &QLineEdit::textChanged, this, [&](const QString& text) {

		//no diagnosis
		if (text.isEmpty()) {
			ui.icdButton->setText("МКБ 10");
			ui.errorLabel->clear();
			return;
		}

		auto& icdCode = ICD10::getCodeFromName(text.toStdString());

		//valid code
		if (icdCode.size()) {
			ui.errorLabel->clear();
			ui.icdButton->setText(icdCode.c_str());
			return;
		}

		//invalid code
		ui.errorLabel->setText("Невалидна МКБ диагноза");
		ui.icdButton->setText("МКБ 10");

		});

	QString toothIndexes[32]{ "18", "17", "16", "15", "14", "13", "12", "11",
						  "21", "22", "23", "24", "25", "26", "27", "28",
						  "38", "37", "36", "35", "34", "33", "32", "31",
						  "41", "42", "43", "44", "45", "46", "47", "48" };

	for (int i = 0; i < 32; i++)
	{
		ui.beginCombo->addItem(toothIndexes[i]);
		ui.endCombo->addItem(toothIndexes[i]);
	}

	//setting autocomplete diagnosis
	Json::Value diagnosisList;

	Json::Reader().parse(Resources::fromPath(":/json/json_diagnosisText.json"), diagnosisList);

	QStringList completionList;
	completionList.reserve(diagnosisList.size());

	for (auto& d : diagnosisList) completionList.append(d.asCString());

	ui.diagDescrEdit->setCompletions(completionList);

	completionList.clear();

	completionList.reserve(ICD10::getDentalICDCodes().size());

	for (auto& d : ICD10::getDentalICDCodes()) {
		completionList.push_back(d.name().c_str());
	}

	ui.icdEdit->setCompletions(completionList);
	ui.icdEdit->completer()->setFilterMode(Qt::MatchContains);
}

QDateEdit* ProcedureInput::qDateEdit()
{
	return ui.dateEdit;
}

AbstractDateEdit* ProcedureInput::dateEdit()
{
	return ui.dateEdit;
}

void ProcedureInput::setCommonData(const CommonData& data, bool hasNhifCode)
{
	ui.diagnosisGroup->show();
	ui.notesGroup->show();
	ui.financingGroup->show();

	ui.icdEdit->setText(data.diagnosis.icd.name().c_str());
	ui.diagDescrEdit->setText(data.diagnosis.additional_descr.c_str());
	ui.notesEdit->setPlainText(data.notes.c_str());
	initFinancingCombo(hasNhifCode);
	setFinancingSource(data.financingSource);
	ui.errorLabel->clear();

}

void ProcedureInput::setParameterData()
{
	ui.paramFrame->show();

	for (auto o : ui.paramFrame->children()) {
		if (o->isWidgetType()) {
			static_cast<QWidget*>(o)->hide();
		}
	}
}

void ProcedureInput::setParameterData(AnesthesiaMinutes minutes)
{
	setParameterData();

	ui.anesthesiaGroup->show();
	ui.anesthesiaSpin->setValue(minutes.minutes);
}

void ProcedureInput::setParameterData(bool supernumeral)
{
	setParameterData();

	ui.rangeCheck->hide();
	ui.toothFrame->show();
	ui.hyperdonticCheckBox->setChecked(supernumeral);
	
}

void ProcedureInput::setParameterData(bool supernumeral, RestorationData restoration)
{
	setParameterData(supernumeral);

	ui.surfaceGroup->show();

	ui.o_check->setChecked(restoration.surfaces[0]);
	ui.m_check->setChecked(restoration.surfaces[1]);
	ui.d_check->setChecked(restoration.surfaces[2]);
	ui.b_check->setChecked(restoration.surfaces[3]);
	ui.l_check->setChecked(restoration.surfaces[4]);
	ui.c_check->setChecked(restoration.surfaces[5]);
	ui.postCheck->setChecked(restoration.post);

	ui.postCheck->setDisabled(m_postDisabled);
}

void ProcedureInput::setParameterData(ConstructionRange range, bool allowSingle)
{
	setParameterData();

	m_allow_singleRange = allowSingle;

	ui.rangeGroup->show();

	ui.beginCombo->setCurrentIndex(range.toothFrom);
	ui.endCombo->setCurrentIndex(range.toothTo);
}

void ProcedureInput::setParameterData(bool supernumeral, ConstructionRange range, bool preferSingle)
{
	m_allow_singleRange = false;

	setParameterData();

	ui.hyperdonticCheckBox->setChecked(supernumeral);
	ui.beginCombo->setCurrentIndex(range.toothFrom);
	ui.endCombo->setCurrentIndex(range.toothTo);

	ui.rangeCheck->show();

	ui.rangeCheck->setChecked(!preferSingle);
	emit ui.rangeCheck->toggled(!preferSingle); //in case it is already checked
}

void ProcedureInput::setParameterData(bool supernumeral, ConstructionRange range, RestorationData r, int preferedIndex)
{
	m_allow_singleRange = false;

	setParameterData(supernumeral, r);
	ui.beginCombo->setCurrentIndex(range.toothFrom);
	ui.endCombo->setCurrentIndex(range.toothTo);

	setParameterData(); //hiding everything...

	ui.constructionTypeGroup->show();
	ui.constructionTypeCombo->setCurrentIndex(preferedIndex);
	emit ui.constructionTypeCombo->currentIndexChanged(preferedIndex);  //in case it is already checked
}

IProcedureInput::ResultData ProcedureInput::getResult()
{
	ResultData result;

	auto diagCode = ui.icdButton->text();

	result.diagnosis.icd = ICD10{ ui.icdButton->text().toStdString() };

	if (result.diagnosis.icd.isValid()) {
		result.diagnosis.additional_descr = ui.diagDescrEdit->getText();
	}

	result.financingSource = getFinancingSource();

	result.notes = ui.notesEdit->getText();

	if (ui.anesthesiaSpin->isVisible()) {
		result.parameters = ui.anesthesiaSpin->value();
	}

	if (ui.hyperdonticCheckBox->isVisible()) {
		result.parameters = ui.hyperdonticCheckBox->isChecked();
	}

	if (ui.surfaceGroup->isVisible()) {
		result.parameters = std::make_pair(ui.hyperdonticCheckBox->isChecked(), getRestorationData());
	}

	if (ui.rangeGroup->isVisible()) {
		result.parameters = getConstructionRange();
	}

	return result;

}

void ProcedureInput::setErrorMsg(const std::string& errorMsg)
{
	for (auto child : children()) {

		if (child->isWidgetType()) {
			static_cast<QWidget*>(child)->setHidden(true);
		}
	}

	ui.errorLabel->setHidden(false);
	ui.errorLabel->setText(errorMsg.c_str());
}

void ProcedureInput::disablePost()
{
	m_postDisabled = true;
	ui.postCheck->hide();
}

std::string ProcedureInput::isValid()
{
	ui.dateEdit->validateInput();

	if (!ui.dateEdit->isValid()) {

		return ui.dateEdit->getValidator()->getErrorMessage();
	}

	if (ui.icdEdit->text().size() 
		&& !ICD10(ui.icdButton->text().toStdString()).isValid())
	{
		return "Невалидна диагноза по МКБ";
	}

	if (ui.surfaceGroup->isVisible()) {

		if (!getRestorationData().isValid()){ return "Изберете поне една повърхност!"; }
	}

	if (ui.rangeGroup->isVisible())
	{

		auto constructionRange = getConstructionRange();

		if (!constructionRange.isFromSameJaw()) {

			return "Началният и крайният зъб за които се отнася процедурата трябва да са от една и съща челюст";
		}

		if (!m_allow_singleRange && constructionRange.getTeethCount() == 1)
		{
			return "Началният и крайният зъб за които се отнася процедурата трябва да са различни";
		}

	}

	return std::string();
}

void ProcedureInput::initFinancingCombo(bool hasNhifCode)
{
	ui.financingCombo->clear();

	ui.financingCombo->addItem("Без финансиране", static_cast<int>(FinancingSource::None));
	ui.financingCombo->addItem(QIcon(":/icons/icon_user.png"), "Пациент", static_cast<int>(FinancingSource::Patient));
	ui.financingCombo->addItem(QIcon(":/icons/icon_phif.png"), "ДЗОФ", static_cast<int>(FinancingSource::PHIF));

	if (hasNhifCode) {
		ui.financingCombo->addItem(QIcon(":/icons/icon_nhif.png"), "НЗОК", static_cast<int>(FinancingSource::NHIF));
	}
}

void ProcedureInput::setFinancingSource(FinancingSource source)
{
	for (int i = 0; i < ui.financingCombo->count(); i++)
	{
		if (static_cast<FinancingSource>(ui.financingCombo->itemData(i).toInt()) == static_cast<FinancingSource>(source)) {
			ui.financingCombo->setCurrentIndex(i);
		}
	}
}

FinancingSource ProcedureInput::getFinancingSource()
{
	return static_cast<FinancingSource>(ui.financingCombo->itemData(ui.financingCombo->currentIndex()).toInt());
}


ConstructionRange ProcedureInput::getConstructionRange()
{
	return ConstructionRange{
				ui.beginCombo->currentIndex(),
				ui.endCombo->currentIndex()
	};
}

RestorationData ProcedureInput::getRestorationData()
{
	return RestorationData{
		{
			ui.o_check->isChecked(),
			ui.m_check->isChecked(),
			ui.d_check->isChecked(),
			ui.b_check->isChecked(),
			ui.l_check->isChecked(),
			ui.c_check->isChecked()
		},
		ui.postCheck->isChecked()
	};
}

ProcedureInput::~ProcedureInput()
{
}

