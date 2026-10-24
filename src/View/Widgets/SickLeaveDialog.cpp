#include "SickLeaveDialog.h"
#include <QPainter>
#include "View/ModalDialogBuilder.h"
#include <json/json.h>
#include "Resources.h"
#include "Presenter/NewDocPresenter.h" 
#include "Database/DbPatient.h" 

SickLeaveDialog::SickLeaveDialog(const SickLeave& notice)
	: QDialog(nullptr)
	, ui(new Ui::SickLeaveDialogClass())
	, m_result(notice)

{
	ui->setupUi(this);

	setWindowTitle("Болничен лист");

	for (auto& [idx, descr] : SickLeave::reasons) {
		ui->reasonCombo->addItem(QString::fromStdString(descr), idx);
	}

	for (auto [idx, descr] : SickLeave::regimes) {
		ui->regimeCombo->addItem(QString::fromStdString(descr), idx);
	}

	for (auto [idx, descr] : SickLeave::notes) {
		ui->noteCombo->addItem(QString::fromStdString(descr), idx);
	}

	for (auto [idx, descr] : SickLeave::relationships) {
		ui->relationshipCombo->addItem(QString::fromStdString(descr), idx);
	}

	connect(ui->icdButton, &QPushButton::clicked, this, [&] {

			auto result = ModalDialogBuilder::icdDialog(ui->icdButton->text().toStdString());

			if (result.isValid()) {
				ui->icdEdit->setText(result.name().c_str());
				ui->icdEdit->setCursorPosition(0);
				ui->icdButton->setText(result.code().c_str());
			}
	});

	connect(ui->caregiverButton, &QPushButton::clicked, this, [&] { addCaregiver(); });

	//setting autocomplete diagnosis
	Json::Value diagnosisList;

	Json::Reader().parse(Resources::fromPath(":/json/json_diagnosisText.json"), diagnosisList);

	QStringList completionList;
	completionList.reserve(ICD10::getDentalICDCodes().size());

	for (auto& d : ICD10::getDentalICDCodes()) {
		completionList.push_back(d.name().c_str());
	}

	ui->icdEdit->setCompletions(completionList);
	ui->icdEdit->completer()->setFilterMode(Qt::MatchContains);
	ui->icdEdit->setErrorLabel(ui->errorLabel);

	connect(ui->icdEdit, &QLineEdit::textChanged, this, [&](const QString& text) {

		auto& icdCode = ICD10::getCodeFromName(text.toStdString());

		ui->icdButton->setText(icdCode.size() ? icdCode.c_str() : "МКБ 10");

	});

	connect(ui->reasonCombo, &QComboBox::currentIndexChanged, this, [&] { caregiverLogic(); });
	connect(ui->noteCombo, &QComboBox::currentIndexChanged, this, [&] { caregiverLogic(); });

	auto setComboIndexByUserValue = [](QComboBox* combo, int index) {

		for (int i = 0; i < combo->count(); i++) {
			if (combo->itemData(i) == index) {
				combo->setCurrentIndex(i);
				return;
			}
		}
	};

	setComboIndexByUserValue(ui->relationshipCombo, notice.relationship);
	setComboIndexByUserValue(ui->reasonCombo, notice.reasonCode);
	setComboIndexByUserValue(ui->noteCombo, notice.noteCode);
	setComboIndexByUserValue(ui->regimeCombo, notice.regimeType);

	ui->cityLineEdit->setCompletions(Ekatte::cityNameToIdx());
	ui->cityLineEdit->setErrorLabel(ui->errorLabel);

	ui->continuationCheck->setChecked(notice.isContinuation);
	ui->date->set_Date(notice.date);
	ui->fromDateEdit->set_Date(notice.startDate);
	ui->toDateEdit->set_Date(notice.endDate);
	ui->checkupDate->set_Date(notice.checkup);
	ui->icdButton->setText(notice.diagnosis.isValid() ? notice.diagnosis.code().c_str() : "МКБ 10");
	ui->icdEdit->setText(notice.diagnosis.name().c_str());
	ui->notesEdit->setPlainText(notice.additionalNote.c_str());
	ui->cityLineEdit->setText(notice.employerCity.getString(false).c_str());
	ui->empolyerEdit->set_Text(notice.employerName);
	ui->professionEdit->set_Text(notice.profession);
	ui->positionEdit->set_Text(notice.position);
	ui->cityLineEdit->setInputValidator(&city_validator);
	ui->empolyerEdit->setInputValidator(&not_empty_validator);
	ui->professionEdit->setInputValidator(&not_empty_validator);
	ui->positionEdit->setInputValidator(&not_empty_validator);
	ui->icdEdit->setInputValidator(&icd_validator);

	connect(ui->toDateEdit, &QDateEdit::dateChanged, this, [&](QDate date) { 
		ui->checkupDate->setDate(date); 
		if (date < ui->fromDateEdit->date()) {
			ui->fromDateEdit->setDate(date);
		}
	});

	connect(ui->fromDateEdit, &QDateEdit::dateChanged, this, [&](QDate date) {
		if (date > ui->toDateEdit->date()) {
			ui->toDateEdit->setDate(date);
		}
	});

	caregiverLogic();

	m_caregiver_rowid = notice.caregiver_rowid;

	ui->relationshipCombo->setDisabled(!m_caregiver_rowid);

	if (m_caregiver_rowid) {
		auto p = DbPatient::get(m_caregiver_rowid);
		ui->caregiverButton->setText(p.firstLastName().c_str());
		
	}

	connect(ui->okButton, &QPushButton::clicked, this, [&] {

		if (!isValid()) return;

		if (m_result->number) {
			ModalDialogBuilder::showMessage("Болничният лист вече е изпратен към НОИ и промените няма да бъдат запазени");
			reject();
			return;
		}

		m_result->employerCity = ui->empolyerEdit->text().toStdString();
		
		m_result->date = ui->date->getDate();
		m_result->startDate = ui->fromDateEdit->getDate();
		m_result->endDate = ui->toDateEdit->getDate();
		m_result->checkup = ui->checkupDate->getDate();

		m_result->diagnosis = ICD10(ui->icdButton->text().toStdString());

		m_result->additionalNote = ui->notesEdit->toPlainText().toStdString();

		m_result->employerCity = Ekatte(ui->cityLineEdit->getText());
		m_result->employerName = ui->empolyerEdit->getText();
		m_result->profession = ui->professionEdit->getText();
		m_result->position = ui->positionEdit->getText();
		m_result->caregiver_rowid = m_caregiver_rowid;
		m_result->regimeType = ui->regimeCombo->itemData(ui->regimeCombo->currentIndex()).toInt();
		m_result->reasonCode = ui->reasonCombo->itemData(ui->reasonCombo->currentIndex()).toInt();
		m_result->noteCode = ui->noteCombo->itemData(ui->noteCombo->currentIndex()).toInt();
		m_result->relationship = ui->relationshipCombo->itemData(ui->relationshipCombo->currentIndex()).toInt();
		m_result->isContinuation = ui->continuationCheck->isChecked();

		accept();

	});

	ui->icdEdit->setFocus();
}

bool SickLeaveDialog::isValid() 
{
	AbstractUIElement* validatableUI[] = {
		ui->icdEdit,
		ui->empolyerEdit,
		ui->cityLineEdit,
		ui->professionEdit,
		ui->positionEdit
	};

	for (auto e : validatableUI) {

		auto valid = e->validateInput();

		if (!valid) {
			e->set_focus();
			return false;
		}
	}

	return true;
}

void SickLeaveDialog::addCaregiver()
{
	auto parent = NewDocPresenter("Избор на получател на болничния лист").open();

	if (!parent) return;

	ui->caregiverButton->setText(parent->firstLastName().c_str());

	m_caregiver_rowid = parent->rowid;

	ui->relationshipCombo->setDisabled(false);

}

void SickLeaveDialog::caregiverLogic()
{
	auto reasonCode = ui->reasonCombo->itemData(ui->reasonCombo->currentIndex()).toInt();
	auto noteCode = m_result->noteCode = ui->noteCombo->itemData(ui->noteCombo->currentIndex()).toInt();

	bool showCaregiver = reasonCode == 26 || reasonCode == 27 || noteCode == 10 || noteCode == 11;

	if (!showCaregiver == ui->caregiverFrame->isHidden()) return;

	ui->caregiverFrame->setHidden(!showCaregiver);

	//simply resetting everything
	ui->relationshipCombo->setDisabled(true);
	m_caregiver_rowid = 0;
	ui->caregiverButton->setText("Добави получател на болничния лист");
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

