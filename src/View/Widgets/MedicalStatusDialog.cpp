#include "MedicalStatusDialog.h"
#include "Presenter/MedicalStatusPresenter.h"

MedicalStatusDialog::MedicalStatusDialog(MedicalStatusPresenter* p)
	: presenter(p), QDialog(nullptr)
{
	ui.setupUi(this);

	setWindowTitle("Медицински статус");

	//ui.allergiesWidget->setName("Алергии");
	ui.currentWidget->setName("Настоящи заболявания");
	ui.pastWidget->setName("Минали заболявания");

	connect(ui.addAllergy, &QPushButton::clicked, [&] {presenter->addAllergy(); });
	connect(ui.removeAllergy, &QPushButton::clicked, [&] {presenter->removeAllergy(ui.allergiesList->currentIndex().row()); });
	connect(ui.editAllergy, &QPushButton::clicked, [&] {presenter->editAllergy(ui.allergiesList->currentIndex().row()); });
	connect(ui.sendHisButton, &QPushButton::clicked, [&] {presenter->sendToHis(ui.allergiesList->currentIndex().row()); });
	connect(ui.getHisButton, &QPushButton::clicked, [&] {presenter->loadAllergiesFromHis(); });
	connect(ui.okButton, &QPushButton::clicked, [&] { presenter->okPressed(); });

}

void MedicalStatusDialog::setPastDiseases(const std::vector<MedicalStatus>& pd)
{
	ui.pastWidget->setMedicalStatus(pd);
}

std::vector<MedicalStatus> MedicalStatusDialog::getPastDiseases()
{
	return ui.pastWidget->getMedicalStatus();
}

void MedicalStatusDialog::setCurrentDiseases(const std::vector<MedicalStatus>& cd)
{
	ui.pastWidget->setMedicalStatus(cd);
}

std::vector<MedicalStatus> MedicalStatusDialog::getCurrentDiseases()
{
	return ui.currentWidget->getMedicalStatus();
}

void MedicalStatusDialog::setAllergies(const std::vector<Allergy>& allergies)
{
	ui.allergiesList->clear();

	for (auto& a : allergies)
	{
		ui.allergiesList->addItem(a.description.c_str());

		if (a.nrn.empty()) return;

		ui.allergiesList->item(ui.allergiesList->count()-1)->setIcon(QIcon(":/icons/icon_his.png"));
	}
}

MedicalStatusDialog::~MedicalStatusDialog()
{}

#include <QPainter>

void MedicalStatusDialog::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}
