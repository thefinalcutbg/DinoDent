#include <QPainter>

#include "PracticeSettings.h"
#include "Model/UserStructs.h"
#include "QtVersion.h"

constexpr int specIdxSize = 5;

constexpr const char* specialties[specIdxSize]{
	"Няма", "64", "61", "62", "68"
};

void PracticeSettings::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

PracticeSettings::PracticeSettings(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	lineEdits[PracticeTextFields::Name] = ui.practiceNameEdit;
	lineEdits[PracticeTextFields::RZI] = ui.rziEdit;
	lineEdits[PracticeTextFields::FirmAddress] = ui.firmAddressEdit;
	lineEdits[PracticeTextFields::ActivityAddress] = ui.activityAddressEdit;
	lineEdits[PracticeTextFields::Bulstat] = ui.bulstatEdit;
	lineEdits[PracticeTextFields::Password] = ui.passEdit;
	lineEdits[PracticeTextFields::VAT] = ui.vatEdit;
	lineEdits[PracticeTextFields::NZOKContract] = ui.contractEdit;
	lineEdits[PracticeTextFields::NZOKShortName] = ui.practiceNameNZOK;
	lineEdits[PracticeTextFields::NraPass] = ui.nraPass;
	lineEdits[PracticeTextFields::SelfInsuredId] = ui.selfInsuredId;

	for (auto lineEdit : lineEdits)
	{
		static_cast<LineEdit*>(lineEdit)->setErrorLabel(ui.errorLabel);
	}

	ui.adminCheck->setHidden(true);
	ui.specialtyCombo->setHidden(true);
	ui.label_13->setHidden(true);

	ui.contractDateEdit->setErrorLabel(ui.errorLabel);

	connect(ui.nzokGroup, &QGroupBox::clicked, [=]{presenter->nzokContractEnabled(ui.nzokGroup->isChecked());});
	
	for (auto& specialty : specialties)
	{
		ui.specialtyCombo->addItem(specialty);
	}

	ui.doctorList->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.doctorList->setSelectionBehavior(QAbstractItemView::SelectRows);

	connect(ui.removeDoctor, &QPushButton::clicked, [=] { presenter->deleteDoctor(); });
	connect(ui.addDoctor, &QPushButton::clicked, [=] {presenter->addDoctor();});
	connect(ui.doctorList, &QListWidget::itemSelectionChanged, [=]() {
			
			int row = ui.doctorList->selectionModel()->currentIndex().row();

			bool hideDetails = row < 0;

			ui.adminCheck->setHidden(hideDetails);
			ui.specialtyCombo->setHidden(hideDetails);
			ui.label_13->setHidden(hideDetails);

			presenter->indexChanged(row);
		
		});
	connect(ui.adminCheck, &QCheckBox::stateChanged, [=](int state) { presenter->setAdminPrivilege(state);});
	connect(ui.specialtyCombo, QtComboIndexChanged, [=](int index) {
		presenter->setDoctorNhifSpecialty(static_cast<NhifSpecialty>(index));
		}
	);

	connect(ui.legalEntityCombo, QtComboIndexChanged, [=](int index) {

			bool notSelfInsured = index;

			ui.selfInsuredId->setHidden(notSelfInsured);
			ui.selfInsuredLabel->setHidden(notSelfInsured);

			ui.selfInsuredId->setInputValidator(
				notSelfInsured ?
				nullptr
				:
				&id_validator
			);

		}
	);


	ui.legalEntityCombo->setCurrentIndex(2);

	ui.activityAddressEdit->setCompletions(Ekatte::cityNameToIdx());

}

PracticeSettings::~PracticeSettings()
{
}

void PracticeSettings::setPractice(const Practice& practice)
{
	ui.practiceNameEdit->setText(QString::fromStdString(practice.name));
	ui.rziEdit->setText(QString::fromStdString(practice.rziCode));
	ui.bulstatEdit->setText(QString::fromStdString(practice.bulstat));
	ui.legalEntityCombo->setCurrentIndex(practice.legal_entity);
	ui.firmAddressEdit->setText(QString::fromStdString(practice.firm_address));
	ui.activityAddressEdit->setText(practice.practice_address.getString(false).c_str());
	ui.vatEdit->setText(QString::fromStdString(practice.vat));
	ui.passEdit->setText(QString::fromStdString(practice.pass));
	ui.selfInsuredId->setText(QString::fromStdString(practice.selfInsuredId));
	
	ui.nzokGroup->setChecked(practice.nhif_contract.has_value());
	if (practice.nhif_contract) {
		ui.contractEdit->setText(QString::fromStdString(practice.nhif_contract.value().contract_no));
		ui.contractDateEdit->set_Date(practice.nhif_contract.value().date);
		ui.practiceNameNZOK->setText(QString::fromStdString(practice.nhif_contract.value().name_short));
		ui.nraPass->setText(practice.nhif_contract->nra_pass.data());
		ui.unfavCheck->setChecked(practice.nhif_contract->unfavourable);
		ui.nssiPass->setText(practice.nhif_contract->nssi_pass.c_str());
	}
}

Practice PracticeSettings::getPractice()
{
	Practice p;

	p.rziCode = ui.rziEdit->getText();
	p.bulstat = ui.bulstatEdit->getText();
	p.firm_address = ui.firmAddressEdit->getText();
	p.practice_address = ui.activityAddressEdit->getText();
	p.legal_entity = ui.legalEntityCombo->currentIndex();
	p.name = ui.practiceNameEdit->getText();
	p.pass = ui.passEdit->getText();
	p.vat = ui.vatEdit->getText();
	p.selfInsuredId = ui.selfInsuredId->getText();

	if (ui.nzokGroup->isChecked())
	{
		NhifContract c;
		c.date = ui.contractDateEdit->getDate();
		c.contract_no = ui.contractEdit->getText();
		c.name_short = ui.practiceNameNZOK->getText();
		c.unfavourable = ui.unfavCheck->isChecked();
		c.nra_pass = ui.nraPass->getText();
		c.nssi_pass = ui.nssiPass->getText();
		p.nhif_contract.emplace(c);
	}

	return p;
}

AbstractLineEdit* PracticeSettings::lineEdit(PracticeTextFields::Field field)
{
	return lineEdits[field];
}

AbstractDateEdit* PracticeSettings::dateEdit()
{
	return ui.contractDateEdit;
}

int PracticeSettings::legalForm()
{
	return ui.legalEntityCombo->currentIndex();
}

void PracticeSettings::setPresenter(PracticeSettingsPresenter* presenter)
{
	this->presenter = presenter;
}

void PracticeSettings::hidePassword()
{
	ui.passEdit->setEchoMode(QLineEdit::EchoMode::Password);
}


void PracticeSettings::setDoctorList(const std::vector<PracticeDoctor>& doctors)
{
	ui.doctorList->clear();

	for (auto doctor : doctors)
	{
		QString postfix = doctor.admin ? " (администратор)" : "";
		ui.doctorList->addItem(QString::fromStdString(doctor.name) + postfix);
	}

	auto count = ui.doctorList->count();

	if (count) {
		ui.doctorList->setCurrentRow(count - 1);
	}
}

void PracticeSettings::setDoctorProperties(bool admin, NhifSpecialty specialty)
{

	//recursion guards
	QSignalBlocker blocker(ui.adminCheck);
	QSignalBlocker blocker2(ui.specialtyCombo);

	ui.adminCheck->setChecked(admin);
	ui.specialtyCombo->setCurrentIndex(static_cast<int>(specialty));

}


void PracticeSettings::replaceCurrentItem(const PracticeDoctor& item)
{
	QString postfix = item.admin ? " (администратор)" : "";
	ui.doctorList->currentItem()->setText(QString::fromStdString(item.name) + postfix);
}
