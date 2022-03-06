#include "PracticeSettings.h"
#include "Model/User/User.h"
#include <QPainter>

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

	for (auto lineEdit : lineEdits)
	{
		static_cast<LineEdit*>(lineEdit)->setErrorLabel(ui.errorLabel);
	}

	ui.contractDateEdit->setErrorLabel(ui.errorLabel);

	connect(ui.nzokGroup, &QGroupBox::clicked, [=]{presenter->nzokContractEnabled(ui.nzokGroup->isChecked());});
	connect(ui.vatGroup, &QGroupBox::clicked, [=]{presenter->vatEnabled(ui.vatGroup->isChecked());});
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
	if (ui.nzokGroup->isChecked()) {
		ui.contractEdit->setText(QString::fromStdString(practice.nzok_contract.value().contract_no));
		ui.contractDateEdit->set_Date(practice.nzok_contract.value().date);
		ui.practiceNameNZOK->setText(QString::fromStdString(practice.nzok_contract.value().name_short));
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

