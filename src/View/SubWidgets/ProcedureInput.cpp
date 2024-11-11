#include "ProcedureInput.h"

#include <QCompleter>
#include <QAbstractItemView>

#include <json/json.h>
#include "View/Widgets/TableViewDialog.h"
#include "Presenter/ProcedureCreator.h"
#include "Resources.h"
#include "View/ModalDialogBuilder.h"
#include "View/Widgets/ICD10Dialog.h"
//to enable prices add QDoubleSpinBox named priceSpin to ui and uncomment the code

ProcedureInput::ProcedureInput(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlag(Qt::WindowMaximizeButtonHint);

	connect(ui.rangeCheck, &QCheckBox::toggled, this, [&](bool checked) {

		ui.rangeGroup->setHidden(!checked);
		ui.hyperdonticCheckBox->setHidden(checked);
		recalculatePrice();
		
	});

	connect(ui.icdButton, &QPushButton::clicked, this, [&] {

			auto result = ModalDialogBuilder::icdDialog(ui.icdButton->text().toStdString());

			if (result.isValid()) {
				ui.icdEdit->setText(result.name().c_str());
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

	connect(ui.beginCombo, &QComboBox::currentIndexChanged, this, [&] { recalculatePrice(); });
	connect(ui.endCombo, &QComboBox::currentIndexChanged, this, [&] { recalculatePrice(); });
/*
	connect(ui.priceSpin, &QDoubleSpinBox::valueChanged, this, [&](double value) {

		auto financingSource = getFinancingSource();

		if (financingSource == FinancingSource::NHIF) { return; }

		if (value == 0) {
			setFinancingSource(FinancingSource::None);
		}
		
		if (value && financingSource == FinancingSource::None) {
			setFinancingSource(FinancingSource::Patient);
		}

	});
*/
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

	valueMultiplier = 1;

	auto codeIsValid = m_code.isValid();

	for (auto child : children()) {

		if (child->isWidgetType()) {
			static_cast<QWidget*>(child)->setHidden(!codeIsValid);
		}
	}

	if (!codeIsValid) return;

	ui.errorLabel->setText("");

	ui.icdEdit->setText(data.diagnosis.icd.name().c_str());
	ui.diagDescrEdit->setText(data.diagnosis.additional_descr.c_str());

	ui.notesEdit->setPlainText(data.notes.c_str());

	//financing logic
	initFinancingCombo(data.code);
	setFinancingSource(data.financingSource);
	
//	ui.priceSpin->setValue(data.price);

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

	if (m_code.getScope() == ProcedureScope::Range || m_code.getScope() == ProcedureScope::Ambi) {

		ui.rangeCheck->setChecked(data.range.has_value());
		emit ui.rangeCheck->toggled(data.range.has_value());

		if (data.range) {

			//if you implement the price value, be sure to block the signals
			ui.beginCombo->setCurrentIndex(data.range->toothFrom);
			ui.endCombo->setCurrentIndex(data.range->toothTo);

			valueMultiplier = data.range->getTeethCount();

		}
	}

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

void ProcedureInput::disableRangeCheck()
{
	ui.rangeCheck->setDisabled(true);
}

IProcedureInput::Data ProcedureInput::getData()
{
	Data result;

	result.code = m_code;

	auto diagCode = ui.icdButton->text();

	result.diagnosis.icd = ICD10{ ui.icdButton->text().toStdString() };

	if (result.diagnosis.icd.isValid()) {
		result.diagnosis.additional_descr = ui.diagDescrEdit->getText();
	}

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
		
		if (result.range->toothFrom > result.range->toothTo) {
			std::swap(result.range->toothFrom, result.range->toothTo);
		}
	}

//	result.value = ui.priceSpin->value();

	return result;
}

std::string ProcedureInput::isValid()
{
	std::string result;

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

		if (!valid){ return "Изберете поне една повърхност!"; }
	}

	if (ui.rangeGroup->isVisible())
	{
		auto type = m_code.type();

		auto from = ui.beginCombo->currentIndex();
		auto to = ui.endCombo->currentIndex();

		if (type != ProcedureType::RemoveCrownOrBridge 
			&& type != ProcedureType::Denture
			&& from == to) 
		{
			return "Началният и крайният зъб за които се отнася процедурата трябва да са различни";
		}

		auto fromSameJaw = (from < 16) == (to < 16);

		if (!fromSameJaw 
			&& type != ProcedureType::MultipleExtraction) {

			return "Началният и крайният зъб за които се отнася процедурата трябва да са от една и съща челюст";
		}
	}

	if (result.size()) {
		ui.errorLabel->setText(result.c_str());
	}

	return result;
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

		break;

	case ProcedureScope::SingleTooth:
		
		ui.hyperdonticCheckBox->show();
		ui.surfaceGroup->setHidden(code.type() != ProcedureType::Restoration);

		ui.anesthesiaGroup->hide();
		ui.rangeCheck->hide();
		ui.rangeGroup->hide();

		break;

	case ProcedureScope::Range:

		ui.surfaceGroup->hide();
		ui.anesthesiaGroup->hide();
		ui.hyperdonticCheckBox->hide();
		ui.surfaceGroup->hide();
		ui.rangeCheck->hide();

		ui.rangeGroup->show();
			
		break;

	case ProcedureScope::Ambi:

		ui.hyperdonticCheckBox->show();
		ui.rangeCheck->show();
		ui.rangeGroup->hide();

		ui.surfaceGroup->hide();
		ui.anesthesiaGroup->hide();

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

	ui.financingCombo->addItem("Без финансиране", static_cast<int>(FinancingSource::None));
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

void ProcedureInput::recalculatePrice()
{
/*
	double currentPrice = ui.priceSpin->value();

	if (currentPrice == 0) return;

	//not recalculating for dentures
	if (m_code.type() == ProcedureType::Denture) return;

	int newRange = ui.rangeGroup->isVisible() ?
		ConstructionRange(
			ui.beginCombo->currentIndex(),
			ui.endCombo->currentIndex()
		).getTeethCount()
		:
		1
	;

	double newPrice = currentPrice / valueMultiplier * newRange;

	ui.priceSpin->setValue(newPrice);
	valueMultiplier = newRange;
*/
}

ProcedureInput::~ProcedureInput()
{
}

