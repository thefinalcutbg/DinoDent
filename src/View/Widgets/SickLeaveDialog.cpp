#include "SickLeaveDialog.h"
#include <QPainter>
#include "View/ModalDialogBuilder.h"
#include <json/json.h>
#include "Resources.h"

SickLeaveDialog::SickLeaveDialog(const SickLeave& notice)
	: QDialog(nullptr)
	, ui(new Ui::SickLeaveDialogClass())
	, m_result(notice)
{
	ui->setupUi(this);

	for (auto& [idx, descr] : SickLeave::reasons) {
		ui->reasonCombo->addItem(QString::fromStdString(descr), idx);
	}

	for (auto [idx, descr] : SickLeave::regimes) {
		ui->regimeCombo->addItem(QString::fromStdString(descr), idx);
	}

	for (auto [idx, descr] : SickLeave::notes) {
		ui->noteCombo->addItem(QString::fromStdString(descr), idx);
	}

	connect(ui->icdButton, &QPushButton::clicked, this, [&] {

			auto result = ModalDialogBuilder::icdDialog(ui->icdButton->text().toStdString());

			if (result.isValid()) {
				ui->icdEdit->setText(result.name().c_str());
				ui->icdEdit->setCursorPosition(0);
				ui->icdButton->setText(result.code().c_str());
			}
	});

	//setting autocomplete diagnosis
	Json::Value diagnosisList;

	Json::Reader().parse(Resources::fromPath(":/json/json_diagnosisText.json"), diagnosisList);


	QStringList completionList;
	completionList.reserve(diagnosisList.size());

	for (auto& d : diagnosisList) completionList.append(d.asCString());

	ui->diagDescrEdit->setCompletions(completionList);

	completionList.clear();

	completionList.reserve(ICD10::getDentalICDCodes().size());

	for (auto& d : ICD10::getDentalICDCodes()) {
		completionList.push_back(d.name().c_str());
	}

	ui->icdEdit->setCompletions(completionList);
	ui->icdEdit->completer()->setFilterMode(Qt::MatchContains);

	connect(ui->icdEdit, &QLineEdit::textChanged, this, [&](const QString& text) {

		//no diagnosis
		if (text.isEmpty()) {
			ui->icdButton->setText("МКБ 10");
			ui->errorLabel->clear();
			return;
		}

		auto& icdCode = ICD10::getCodeFromName(text.toStdString());

		//valid code
		if (icdCode.size()) {
			ui->errorLabel->clear();
			ui->icdButton->setText(icdCode.c_str());
			return;
		}

		//invalid code
		ui->errorLabel->setText("Невалидна МКБ диагноза");
		ui->icdButton->setText("МКБ 10");

	});

	auto setComboIndexByUserValue = [](QComboBox* combo, int index) {

		for (int i = 0; i < combo->count(); i++) {
			if (combo->itemData(i) == index) {
				combo->setCurrentIndex(i);
				return;
			}
		}
	};

	setComboIndexByUserValue(ui->reasonCombo, notice.reasonCode);
	setComboIndexByUserValue(ui->noteCombo, notice.noteCode);
	setComboIndexByUserValue(ui->regimeCombo, notice.regimeType);

	ui->cityLineEdit->setCompletions(Ekatte::cityNameToIdx());
	ui->cityLineEdit->setErrorLabel(ui->errorLabel);

	ui->continuationCheck->setChecked(notice.isContinuation);
	ui->date->set_Date(notice.date);
	ui->fromDateEdit->set_Date(notice.startDate);
	ui->toDateEdit->set_Date(notice.endDate);
	ui->icdButton->setText(notice.diagnosis.icd.isValid() ? notice.diagnosis.icd.code().c_str() : "Изберете");
	ui->icdEdit->setText(notice.diagnosis.icd.name().c_str());
	ui->diagDescrEdit->setText(notice.diagnosis.additional_descr.c_str());
	ui->notesEdit->setPlainText(notice.additionalNote.c_str());
	ui->cityLineEdit->setText(notice.employerCity.getString(false).c_str());
	ui->empolyerEdit->set_Text(notice.employerName);
	ui->professionEdit->set_Text(notice.profession);
	ui->positionEdit->set_Text(notice.position);

	ui->cityLineEdit->setInputValidator(&city_validator);
	ui->empolyerEdit->setInputValidator(&not_empty_validator);
	ui->icdEdit->setInputValidator(&not_empty_validator);

	connect(ui->okButton, &QPushButton::clicked, this, [&] {

		if (!isValid()) return;

		m_result->employerCity = ui->empolyerEdit->text().toStdString();
		
		m_result->date = ui->date->getDate();
		m_result->startDate = ui->fromDateEdit->getDate();
		m_result->endDate = ui->toDateEdit->getDate();

		m_result->diagnosis.icd = ICD10(ui->icdButton->text().toStdString());
		m_result->diagnosis.additional_descr = ui->diagDescrEdit->text().toStdString();

		m_result->additionalNote = ui->notesEdit->toPlainText().toStdString();

		m_result->employerCity = Ekatte(ui->cityLineEdit->getText());
		m_result->employerName = ui->empolyerEdit->getText();
		m_result->profession = ui->professionEdit->getText();
		m_result->position = ui->positionEdit->getText();

		accept();

	});
}

bool SickLeaveDialog::isValid() 
{
	if (!ui->cityLineEdit->validateInput()) { ui->cityLineEdit->setFocus(); return false; }
	if (!ui->empolyerEdit->validateInput()) { ui->empolyerEdit->setFocus(); return false; }
}

void SickLeaveDialog::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}

std::optional<SickLeave> SickLeaveDialog::getResult()
{
	auto result = exec();

	if (result != QDialog::Accepted) {
		m_result.reset();
	}

	return m_result;
}

SickLeaveDialog::~SickLeaveDialog()
{
	delete ui;
}

