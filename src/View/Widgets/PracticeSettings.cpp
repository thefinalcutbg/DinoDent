#include "PracticeSettings.h"
#include "Model/UserStructs.h"
#include <QPainter>
#include <qdebug.h>
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
	lineEdits[PracticeTextFields::Address] = ui.activityAddressEdit;
	lineEdits[PracticeTextFields::ActivityAddress] = ui.activityAddressEdit;
	lineEdits[PracticeTextFields::Bulstat] = ui.bulstatEdit;
	lineEdits[PracticeTextFields::Password] = ui.passEdit;
	lineEdits[PracticeTextFields::VAT] = ui.vatEdit;
	lineEdits[PracticeTextFields::NZOKContract] = ui.contractEdit;
	lineEdits[PracticeTextFields::NZOKShortName] = ui.practiceNameNZOK;
	lineEdits[PracticeTextFields::NraPass] = ui.nraPass;

	for (auto lineEdit : lineEdits)
	{
		static_cast<LineEdit*>(lineEdit)->setErrorLabel(ui.errorLabel);
	}

	ui.adminCheck->setHidden(true);
	ui.specialtyCombo->setHidden(true);
	ui.label_13->setHidden(true);

	ui.contractDateEdit->setErrorLabel(ui.errorLabel);

	connect(ui.nzokGroup, &QGroupBox::clicked, [=]{presenter->nzokContractEnabled(ui.nzokGroup->isChecked());});
	connect(ui.vatGroup, &QGroupBox::clicked, [=]{presenter->vatEnabled(ui.vatGroup->isChecked());});
	
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
	connect(ui.specialtyCombo, &QComboBox::currentIndexChanged, [=](int index) {
		presenter->setDoctorNhifSpecialty(static_cast<NhifSpecialty>(index));
		}
	);

}

PracticeSettings::~PracticeSettings()
{
}

void PracticeSettings::setPractice(const Practice& practice)
{
	ui.practiceNameEdit->setText(QString::fromStdString(practice.name));
	ui.rziEdit->setText(QString::fromStdString(practice.rziCode));
	ui.bulstatEdit->setText(QString::fromStdString(practice.bulstat));
	ui.comboBox->setCurrentIndex(practice.legal_entity);
	ui.addressEdit->setText(QString::fromStdString(practice.firm_address));
	ui.activityAddressEdit->setText(QString::fromStdString(practice.practice_address));
	ui.vatGroup->setChecked(!practice.vat.empty());
	ui.vatEdit->setText(QString::fromStdString(practice.vat));
	ui.passEdit->setText(QString::fromStdString(practice.pass));
	
	ui.nzokGroup->setChecked(practice.nzok_contract.has_value());
	if (practice.nzok_contract) {
		ui.contractEdit->setText(QString::fromStdString(practice.nzok_contract.value().contract_no));
		ui.contractDateEdit->set_Date(practice.nzok_contract.value().date);
		ui.practiceNameNZOK->setText(QString::fromStdString(practice.nzok_contract.value().name_short));
		ui.nraPass->setText(practice.nzok_contract->nra_pass.data());
	}
}

Practice PracticeSettings::getPractice()
{
	Practice p;

	p.rziCode = ui.rziEdit->getText();
	p.bulstat = ui.bulstatEdit->getText();
	p.firm_address = ui.addressEdit->getText();
	p.practice_address = ui.activityAddressEdit->getText();
	p.legal_entity = ui.comboBox->currentIndex();
	p.name = ui.practiceNameEdit->getText();
	p.pass = ui.passEdit->getText();

	if (ui.nzokGroup->isChecked())
	{
		NzokContract c;
		c.date = ui.contractDateEdit->getDate();
		c.contract_no = ui.contractEdit->getText();
		c.name_short = ui.practiceNameNZOK->getText();
		c.nra_pass = ui.nraPass->getText();
		p.nzok_contract.emplace(c);
	}

	if (ui.vatGroup->isChecked())
	{
		p.vat = ui.vatEdit->getText();
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
	return ui.comboBox->currentIndex();
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
		QString postfix = doctor.admin ? u8" (администратор)" : "";
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
	QString postfix = item.admin ? u8" (администратор)" : "";
	ui.doctorList->currentItem()->setText(QString::fromStdString(item.name) + postfix);
}
