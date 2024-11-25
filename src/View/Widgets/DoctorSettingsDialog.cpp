#include "DoctorSettingsDialog.h"
#include "Presenter/DoctorDialogPresenter.h"
#include <QPainter>
#include "TableViewDialog.h"

void DoctorSettingsDialog::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

DoctorSettingsDialog::DoctorSettingsDialog(DoctorDialogPresenter& presenter, QWidget *parent)
	: QDialog(parent), presenter(presenter)
{
	ui.setupUi(this);

	setWindowTitle("Лекар");
	setWindowIcon(QIcon{ ":/icons/icon_user.png" });

	lineEdits[DoctorFields::FirstName] = ui.fNameEdit;
	lineEdits[DoctorFields::MiddleName] = ui.mNameEdit;
	lineEdits[DoctorFields::LastName] = ui.lNameEdit;
	lineEdits[DoctorFields::Phone] = ui.phoneEdit;
	lineEdits[DoctorFields::LPK] = ui.lpkEdit;
	lineEdits[DoctorFields::Password] = ui.passEdit;

	auto phoneValidator = new QRegularExpressionValidator(QRegularExpression("[0-9-+]+"), this);
	ui.phoneEdit->QLineEdit::setValidator(phoneValidator);

	for (auto& lineEdit : lineEdits)
	{
		lineEdit->setErrorLabel(ui.errorLabel);
	}

    connect(ui.okButton, &QPushButton::clicked, [&] {presenter.okPressed();});

    connect(ui.lpkEdit, &QLineEdit::textChanged, this,
        [&] {
			if (ui.lpkEdit->isValid())
                presenter.validLPK(ui.lpkEdit->getText());
		});


    connect(ui.specialtyButton, &QPushButton::clicked, this,
        [&] {

				TableViewDialog d(his_specialtyModel, 1, ui.specialtyButton->text().toStdString());
				d.setWindowTitle("Избор на специалност дентална медицина");
				d.exec();

				auto result = d.getResult();

				if (d.getResult().size()) {
					ui.specialtyButton->setText(result.c_str());
					ui.specialtyButton->adjustSize();
				}
		}
	);

    presenter.setView(this);
}

void DoctorSettingsDialog::setDoctor(const Doctor& doctor)
{
	ui.passEdit->setEchoMode(QLineEdit::EchoMode::Password);

	ui.lpkEdit->set_Text(doctor.LPK);
	ui.fNameEdit->set_Text(doctor.fname);
	ui.mNameEdit->set_Text(doctor.mname);
	ui.lNameEdit->set_Text(doctor.lname);
	ui.phoneEdit->set_Text(doctor.phone);
	ui.passEdit->set_Text(doctor.pass);
	ui.severalRHIFcheck->setChecked(doctor.severalRHIF);

	if (doctor.pass.empty()) ui.passEdit->setEchoMode(QLineEdit::Normal);

	auto specialtyName = doctor.hisSpecialty.getName();

	ui.specialtyButton->setText(
		specialtyName.empty() ? "Изберете специалност" : specialtyName.c_str()
	);

	ui.specialtyButton->adjustSize();

}

Doctor DoctorSettingsDialog::getDoctor()
{
	Doctor doctor;
	doctor.LPK = ui.lpkEdit->getText();
	doctor.fname = ui.fNameEdit->getText();
	doctor.mname = ui.mNameEdit->getText();
	doctor.lname = ui.lNameEdit->getText();
	doctor.phone = ui.phoneEdit->getText();
	doctor.hisSpecialty = ui.specialtyButton->text().toStdString();
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

	ui.specialtyButton->setDisabled(true);

	ui.severalRHIFcheck->setDisabled(true);

	ui.passEdit->setEchoMode(QLineEdit::EchoMode::Password);
}


DoctorSettingsDialog::~DoctorSettingsDialog()
{
}
