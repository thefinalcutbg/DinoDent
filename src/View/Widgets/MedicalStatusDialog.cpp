#include "MedicalStatusDialog.h"
#include "Presenter/MedicalStatusPresenter.h"
#include "View/CommonIcon.h"
#include "Model/User.h"

#include <QPainter>


int MedicalStatusDialog::getAllergyIndex()
{
	if (!ui.allergiesList->selectedItems().size()) return -1;

	return 	ui.allergiesList->selectionModel()->currentIndex().row();
}

MedicalStatusDialog::MedicalStatusDialog(MedicalStatusPresenter* p)
    : QDialog(nullptr), presenter(p)
{
	ui.setupUi(this);

	setWindowTitle("Медицински статус");

	//ui.allergiesWidget->setName("Алергии");
	ui.currentWidget->setName("Настоящи заболявания");
	ui.pastWidget->setName("Минали заболявания");

	ui.sendHisButton->setIcon(QIcon(":/icons/icon_his.png"));
	ui.getHisButton->setIcon(QIcon(":/icons/icon_his.png"));

    connect(ui.addAllergy, &QPushButton::clicked, this, [&] {presenter->addAllergy(); });
    connect(ui.removeAllergy, &QPushButton::clicked, this, [&] {presenter->removeAllergy(getAllergyIndex()); });
    connect(ui.editAllergy, &QPushButton::clicked, this, [&] {presenter->editAllergy(getAllergyIndex()); });
    connect(ui.sendHisButton, &QPushButton::clicked, this, [&] {presenter->sendAllergyToHis(getAllergyIndex()); });
    connect(ui.getHisButton, &QPushButton::clicked, this, [&] {presenter->loadAllergiesFromHis(); });

	connect(ui.allergiesList, &QListWidget::doubleClicked, this, [&]{ presenter->editAllergy(getAllergyIndex()); });

    connect(ui.allergiesList, &QListWidget::itemSelectionChanged, this, [&] {
			
		bool noSelection = !ui.allergiesList->selectedItems().size();
		ui.editAllergy->setDisabled(noSelection);
		ui.removeAllergy->setDisabled(noSelection);
		ui.sendHisButton->setDisabled(noSelection);
/*
		if (noSelection) {
			ui.removeAllergy->setIcon(QIcon());
			return;
		}

		ui.removeAllergy->setIcon(
			ui.allergiesList->currentItem()->icon().isNull() ?
			QIcon() : QIcon(":/icons/icon_his.png")
		);
*/
	});

    emit ui.allergiesList->itemSelectionChanged();

    ui.pisConditionButton->setIcon(QIcon(CommonIcon::getPixmap(CommonIcon::NHIF)));
    ui.hisConditionButton->setIcon(QIcon(CommonIcon::getPixmap(CommonIcon::HIS)));

    connect(ui.pisConditionButton, &QPushButton::clicked,  this,[&]{ presenter->loadICDFromNHIS(); });
    connect(ui.hisConditionButton, &QPushButton::clicked,  this,[&]{ presenter->loadICDFromHIS(); });

    ui.pisConditionButton->setHidden(!User::hasNhifContract());

}

void MedicalStatusDialog::setPastDiseases(const std::vector<ICD10>& pd)
{
	ui.pastWidget->setMedicalStatus(pd);
}

std::vector<ICD10> MedicalStatusDialog::getPastDiseases()
{
	return ui.pastWidget->getMedicalStatus();
}

void MedicalStatusDialog::setCurrentDiseases(const std::vector<ICD10>& cd)
{
	ui.currentWidget->setMedicalStatus(cd);
}

std::vector<ICD10> MedicalStatusDialog::getCurrentDiseases()
{
	return ui.currentWidget->getMedicalStatus();
}

void MedicalStatusDialog::setAllergies(const std::vector<Allergy>& allergies)
{
	int idx = getAllergyIndex();

	ui.allergiesList->clear();

	for (auto& a : allergies)
	{

		QListWidgetItem* item = new QListWidgetItem;
		
		QString description = a.description.c_str();

		if (a.nrn.size())
		{
			item->setIcon(QIcon(":/icons/icon_his.png"));
			if (a.edited) description += "*";
		}

		item->setText(description);
		ui.allergiesList->addItem(item);	
	}

	ui.allergiesList->setCurrentRow(idx);
}

MedicalStatusDialog::~MedicalStatusDialog()
{}

void MedicalStatusDialog::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}
