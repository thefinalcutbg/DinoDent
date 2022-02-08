#include "DoctorSettingsDialog.h"
#include "Presenter/DoctorDialogPresenter/DoctorDialogPresenter.h"
#include <unordered_map>
#include <QPainter>

constexpr int specIdxSize = 5;
constexpr int specArray[specIdxSize]
{ 60, 62, 63, 64, 68 };

void DoctorSettingsDialog::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.fillRect(rect(), QColor(Qt::white));
}

DoctorSettingsDialog::DoctorSettingsDialog(DoctorDialogPresenter* presenter, QWidget *parent)
	: QDialog(parent), presenter(presenter)
{
	ui.setupUi(this);

	for (auto& specialty : specArray)
	{
		ui.specialty->addItem(QString::number(specialty));
	}

	lineEdits[DoctorFields::EGN] = ui.egnLineEdit;
	lineEdits[DoctorFields::FirstName] = ui.fNameEdit;
	lineEdits[DoctorFields::LastName] = ui.lNameEdit;
	lineEdits[DoctorFields::LPK] = ui.lpkEdit;
	lineEdits[DoctorFields::Password] = ui.passEdit;

	for (auto& lineEdit : lineEdits)
	{
		lineEdit->setErrorLabel(ui.errorLabel);
	}

	connect(ui.okButton, &QPushButton::clicked, [=] {presenter->okPressed();});

	presenter->setView(this);
}

void DoctorSettingsDialog::setDoctor(const Doctor& doctor)
{
	int specialtyIndex{ -1 };

	for (int i = 0; i < specIdxSize; i++)
	{
		if (doctor.specialty == specArray[i]){
			specialtyIndex = i;
			break;
		}
	}

	ui.specialty->setCurrentIndex(specialtyIndex);
	ui.egnLineEdit->set_Text(doctor.egn);
	ui.lpkEdit->set_Text(doctor.LPK);
	ui.fNameEdit->set_Text(doctor.fname);
	ui.lNameEdit->set_Text(doctor.lname);
	ui.passEdit->set_Text(doctor.pass);
}

Doctor DoctorSettingsDialog::getDoctor()
{
	Doctor doctor;
	doctor.egn = ui.egnLineEdit->getText();
	doctor.LPK = ui.lpkEdit->getText();
	doctor.specialty = specArray[ui.specialty->currentIndex()];
	doctor.fname = ui.fNameEdit->getText();
	doctor.lname = ui.lNameEdit->getText();
	doctor.pass = ui.passEdit->getText();

	return doctor;
}


DoctorSettingsDialog::~DoctorSettingsDialog()
{
}
