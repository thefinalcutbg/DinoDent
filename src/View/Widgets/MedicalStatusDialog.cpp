#include "MedicalStatusDialog.h"

MedicalStatusDialog::MedicalStatusDialog(const MedicalStatuses& s, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Медицински статус");

	ui.allergiesWidget->setName("Алергии");
	ui.currentWidget->setName("Настоящи заболявания");
	ui.pastWidget->setName("Минали заболявания");

	ui.allergiesWidget->setMedicalStatus(s.allergies);
	ui.currentWidget->setMedicalStatus(s.condition);
	ui.pastWidget->setMedicalStatus(s.history);

	connect(ui.okButton, &QPushButton::clicked, [&] { accept(); });
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

#include <QPainter>

void MedicalStatusDialog::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}
