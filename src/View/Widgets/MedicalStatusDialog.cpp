#include "MedicalStatusDialog.h"

MedicalStatusDialog::MedicalStatusDialog(const MedicalStatuses& s, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.allergiesWidget->setName("Алергии");
	ui.currentWidget->setName("Настоящи заболявания");
	ui.pastWidget->setName("Минали заболявания");

	connect(ui.okButton, &QPushButton::clicked, [this] { accept(); });
}

MedicalStatuses MedicalStatusDialog::getResult()
{
	return MedicalStatuses{
		ui.allergiesWidget->getMedicalStatus(),
		ui.currentWidget->getMedicalStatus(),
		ui.pastWidget->getMedicalStatus()
	};
}

MedicalStatusDialog::~MedicalStatusDialog()
{}
