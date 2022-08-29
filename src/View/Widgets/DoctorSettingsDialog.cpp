#include "DoctorSettingsDialog.h"
#include "Presenter/DoctorDialogPresenter.h"
#include <unordered_map>
#include <QPainter>


void DoctorSettingsDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

DoctorSettingsDialog::DoctorSettingsDialog(DoctorDialogPresenter* presenter, QWidget *parent)
	: QDialog(parent), presenter(presenter)
{
	ui.setupUi(this);

	setWindowTitle(u8"Доктор");
	setWindowIcon(QIcon{ ":/icons/icon_user.png" });

	lineEdits[DoctorFields::EGN] = ui.egnLineEdit;
	lineEdits[DoctorFields::FirstName] = ui.fNameEdit;
	lineEdits[DoctorFields::MiddleName] = ui.mNameEdit;
	lineEdits[DoctorFields::LastName] = ui.lNameEdit;
	lineEdits[DoctorFields::Phone] = ui.phoneEdit;
	lineEdits[DoctorFields::LPK] = ui.lpkEdit;
	lineEdits[DoctorFields::Password] = ui.passEdit;

	for (auto& lineEdit : lineEdits)
	{
		lineEdit->setErrorLabel(ui.errorLabel);
	}

	connect(ui.okButton, &QPushButton::clicked, [=] {presenter->okPressed();});

	connect(ui.lpkEdit, &QLineEdit::textEdited,
		[=] {
			if (ui.lpkEdit->isValid())
				presenter->validLPK(ui.lpkEdit->getText());
		});

	presenter->setView(this);
}

void DoctorSettingsDialog::setDoctor(const Doctor& doctor)
{
	ui.passEdit->setEchoMode(QLineEdit::EchoMode::Password);

	ui.egnLineEdit->set_Text(doctor.egn);
	ui.lpkEdit->set_Text(doctor.LPK);
	ui.fNameEdit->set_Text(doctor.fname);
	ui.mNameEdit->set_Text(doctor.mname);
	ui.lNameEdit->set_Text(doctor.lname);
	ui.phoneEdit->set_Text(doctor.phone);
	ui.passEdit->set_Text(doctor.pass);
	ui.severalRHIFcheck->setChecked(doctor.severalRHIF);
}

Doctor DoctorSettingsDialog::getDoctor()
{
	Doctor doctor;
	doctor.egn = ui.egnLineEdit->getText();
	doctor.LPK = ui.lpkEdit->getText();
	doctor.fname = ui.fNameEdit->getText();
	doctor.mname = ui.mNameEdit->getText();
	doctor.lname = ui.lNameEdit->getText();
	doctor.phone = ui.phoneEdit->getText();
	doctor.pass = ui.passEdit->getText();
	doctor.severalRHIF = ui.severalRHIFcheck->isChecked();

	return doctor;
}

void DoctorSettingsDialog::setToReadOnly()
{
	for (auto field : lineEdits)
	{
		field->setReadOnly(true);
	}

	ui.severalRHIFcheck->setDisabled(true);

	ui.passEdit->setEchoMode(QLineEdit::EchoMode::Password);
}


DoctorSettingsDialog::~DoctorSettingsDialog()
{
}
