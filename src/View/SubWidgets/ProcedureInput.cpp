#include "ProcedureInput.h"

#include <QCompleter>
#include <QAbstractItemView>

#include <json/json.h>
#include "View/Widgets/TableViewDialog.h"
#include "Presenter/ProcedureCreator.h"
#include "Resources.h"
#include "Model/ICD10.h"

ProcedureInput::ProcedureInput(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlag(Qt::WindowMaximizeButtonHint);

	connect(ui.rangeCheck, &QCheckBox::toggled, this, [&](bool checked) {

		ui.rangeGroup->setHidden(!checked);
		ui.hyperdonticCheckBox->setHidden(checked);

	});

	connect(ui.icdButton, &QPushButton::clicked, this, [&] {

		TableViewDialog d(full_icd, 0, ICD10::getCodeFromName(ui.icdEdit->text().toStdString()));
		d.setWindowTitle("Международна класификация на болестите");
		d.exec(); 

		auto code = d.getResult();

		if (code.size()) {
			ui.icdEdit->setText(QString::fromStdString(ICD10::getDescriptionFromICDCode(code)));
		}
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

void ProcedureInput::setData(const Data& data)
{
	m_code = data.code;

	auto codeIsValid = m_code.isValid();

	for (auto child : children()) {

		if (child->isWidgetType()) {
			static_cast<QWidget*>(child)->setHidden(!codeIsValid);
		}
	}

	if (!codeIsValid) return;

	ui.errorLabel->setText("");

	//ui.diagCombo->setCurrentIndex(data.diagnosis.index());
	ui.diagDescrEdit->setText(data.diagnosis.description.c_str());

	ui.notesEdit->setPlainText(data.notes.c_str());

	//financing logic
	initFinancingCombo(data.code);
	setFinancingSource(data.financingSource);

	initView(data.code);

	if (std::holds_alternative<RestorationData>(data.param) 
		&& data.code.getScope() == ProcedureScope::SingleTooth
		) 
	{

		auto& restoration = std::get<RestorationData>(data.param);

		ui.o_check->setChecked(restoration.surfaces[0]);
		ui.m_check->setChecked(restoration.surfaces[1]);
		ui.d_check->setChecked(restoration.surfaces[2]);
		ui.b_check->setChecked(restoration.surfaces[3]);
		ui.l_check->setChecked(restoration.surfaces[4]);
		ui.c_check->setChecked(restoration.surfaces[5]);
		ui.postCheck->setChecked(restoration.post);
	}

	if (std::holds_alternative<AnesthesiaMinutes>(data.param)
		&& data.code.getScope() == ProcedureScope::AllOrNone
	) 
	{

		ui.anesthesiaSpin->setValue(std::get<AnesthesiaMinutes>(data.param).minutes);
	}

	ui.hyperdonticCheckBox->setChecked(data.hyperdontic);

	//range logic

	ui.rangeCheck->blockSignals(true);

	ui.rangeCheck->setChecked(false);

	if (data.range) {

		ui.beginCombo->setCurrentIndex(data.range->tooth_begin);
		ui.endCombo->setCurrentIndex(data.range->tooth_end);
		
		if (data.code.getScope() == ProcedureScope::Ambi && data.code.type() != ProcedureType::Crown) {
			ui.rangeCheck->setChecked(true);
		}
	}

	ui.rangeCheck->blockSignals(false);

	if (m_postDisabled) disablePost();
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

IProcedureInput::Data ProcedureInput::getData()
{
	Data result;

	result.code = m_code;

	result.diagnosis = Diagnosis{
		0,
		ui.diagDescrEdit->getText()
	};

	result.financingSource = getFinancingSource();

	result.notes = ui.notesEdit->getText();

	//getting data based on widget that is visible

	if (ui.hyperdonticCheckBox->isVisible()) {
		result.hyperdontic = ui.hyperdonticCheckBox->isChecked();
	}

	if (ui.surfaceGroup->isVisible()) {
		result.param = RestorationData{
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

	if (ui.anesthesiaGroup->isVisible()) {
		result.param = AnesthesiaMinutes{ ui.anesthesiaSpin->value() };
	}

	if (ui.beginCombo->isVisible()) {
		result.range = { ui.beginCombo->currentIndex(), ui.endCombo->currentIndex() };
		
		if (result.range->tooth_begin > result.range->tooth_end) {
			std::swap(result.range->tooth_begin, result.range->tooth_end);
		}
	}

	if (ui.quadrantGroup->isVisible()) {

		std::pair<QRadioButton*, ConstructionRange> quadrantRange[4] =
		{
			{ui.quadrantFrist, {0,7}},
			{ui.quadrantSecond,{8,15}},
			{ui.quadrantThird,{16,23}},
			{ui.quadrantFourth,{24,31}}
		};

		for (auto& [radioQ, range] : quadrantRange) {
			if (radioQ->isChecked()) {
				result.range = range;
			}
		}
	}

	if (ui.jawGroup->isVisible()) {

		ConstructionRange jawRange[3] = {
			{0,15},
			{16,31},
			{0,31},
		};

		result.range = jawRange[ui.jawComboBox->currentIndex()];
	}

	return result;
}

std::string ProcedureInput::isValid()
{
	if (ui.errorLabel->text().size()) {
		return ui.errorLabel->text().toStdString();
	}

	if (ui.surfaceGroup->isVisible()) {

		bool valid = false;

		QCheckBox* surfaces[] = {
			ui.o_check,
			ui.m_check,
			ui.d_check,
			ui.b_check,
			ui.l_check,
			ui.c_check,
		};

		for (auto& s : surfaces)
		{
			if (s->isChecked()) {
				valid = true;
			}
		}
		
		if (!valid) return "Изберете поне една повърхност!";
	}

	if (ui.rangeGroup->isVisible())
	{
		auto type = m_code.type();

		auto from = ui.beginCombo->currentIndex();
		auto to = ui.endCombo->currentIndex();

		auto fromSameJaw = (from < 16) == (to < 16);

		if (!fromSameJaw) {;

			return "Невалидна дължина на протетичната конструкция";
		}
		
		if (type != ProcedureType::RemoveCrownOrBridge && from == to) 
		{
			return "Началният и крайният зъб за които се отнася процедурата трябва да са различни";
		}
	}

	return {};
	
}

void ProcedureInput::initView(const ProcedureCode& code)
{

	ui.rangeCheck->setText(
		code.type() == ProcedureType::Crown ?
		"Блок корони"
		:
		"Многочленна конструкция"
	);

	switch (code.getScope()) 
	{

	case ProcedureScope::AllOrNone:

		ui.anesthesiaGroup->setHidden(code.type() != ProcedureType::Anesthesia);

		ui.hyperdonticCheckBox->hide();
		ui.rangeCheck->hide();
		ui.rangeGroup->hide();
		ui.surfaceGroup->hide();
		ui.quadrantGroup->hide();
		ui.jawGroup->hide();

		break;

	case ProcedureScope::SingleTooth:
		
		ui.hyperdonticCheckBox->show();
		ui.surfaceGroup->setHidden(code.type() != ProcedureType::Restoration);

		ui.anesthesiaGroup->hide();
		ui.rangeCheck->hide();
		ui.rangeGroup->hide();
		ui.quadrantGroup->hide();
		ui.jawGroup->hide();

		break;

	case ProcedureScope::Range:

		ui.surfaceGroup->hide();
		ui.anesthesiaGroup->hide();
		ui.hyperdonticCheckBox->hide();
		ui.quadrantGroup->hide();
		ui.rangeCheck->hide();
		ui.rangeGroup->hide();
		ui.surfaceGroup->hide();
		ui.jawGroup->hide();

		switch (code.type())
		{
			case ProcedureType::DepuratioQuadrant:

				ui.quadrantGroup->show();

				break;

			case ProcedureType::MultipleExtraction:

				ui.jawGroup->show();

				break;

			default:

				ui.rangeGroup->show();
		}
		break;

		case ProcedureScope::Ambi:

			ui.hyperdonticCheckBox->show();
			ui.rangeCheck->show();
			ui.rangeGroup->hide();

			ui.surfaceGroup->hide();
			ui.anesthesiaGroup->hide();
			ui.quadrantGroup->hide();
			ui.jawGroup->hide();

			switch (code.type())
			{
			case ProcedureType::Crown:
				ui.rangeCheck->setText("Блок корони");
				break;
			case ProcedureType::CrownOrBridge:
				ui.rangeCheck->setText("Многочленна конструкция");
				break;
			}


		break;
	}
}

void ProcedureInput::initFinancingCombo(const ProcedureCode& code)
{
	ui.financingCombo->clear();

	ui.financingCombo->addItem("Няма", static_cast<int>(FinancingSource::None));
	ui.financingCombo->addItem(QIcon(":/icons/icon_user.png"), "Пациент", static_cast<int>(FinancingSource::Patient));
	ui.financingCombo->addItem(QIcon(":/icons/icon_phif.png"), "ДЗОФ", static_cast<int>(FinancingSource::PHIF));

	if (code.nhifCode()) {
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

ProcedureInput::~ProcedureInput()
{
}

