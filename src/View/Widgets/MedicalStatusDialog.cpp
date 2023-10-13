#include "MedicalStatusDialog.h"
#include "Presenter/MedicalStatusPresenter.h"


#include <QPainter>


int MedicalStatusDialog::getAllergyIndex()
{
	if (!ui.allergiesList->selectedItems().size()) return -1;

	return 	ui.allergiesList->selectionModel()->currentIndex().row();
}

MedicalStatusDialog::MedicalStatusDialog(MedicalStatusPresenter* p)
	: presenter(p), QDialog(nullptr)
{
	ui.setupUi(this);

	setWindowTitle("Медицински статус");

	//ui.allergiesWidget->setName("Алергии");
	ui.currentWidget->setName("Настоящи заболявания");
	ui.pastWidget->setName("Минали заболявания");

	ui.sendHisButton->setIcon(QIcon(":/icons/icon_his.png"));
	ui.getHisButton->setIcon(QIcon(":/icons/icon_his.png"));

	connect(ui.addAllergy, &QPushButton::clicked, [&] {presenter->addAllergy(); });
	connect(ui.removeAllergy, &QPushButton::clicked, [&] {presenter->removeAllergy(getAllergyIndex()); });
	connect(ui.editAllergy, &QPushButton::clicked, [&] {presenter->editAllergy(getAllergyIndex()); });
	connect(ui.sendHisButton, &QPushButton::clicked, [&] {presenter->sendAllergyToHis(getAllergyIndex()); });
	connect(ui.getHisButton, &QPushButton::clicked, [&] {presenter->loadAllergiesFromHis(); });

	connect(ui.allergiesList, &QListWidget::doubleClicked, this, [&]{ presenter->editAllergy(getAllergyIndex()); });

	connect(ui.allergiesList, &QListWidget::itemSelectionChanged, [&] {
			
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

	ui.allergiesList->itemSelectionChanged();

	QPushButton* b = new QPushButton("Диагнози в рецептурна книжка");
	b->setIcon(QIcon(":/icons/icon_nhif.png"));

	connect(b, &QPushButton::clicked, [&]{ presenter->loadICDFromNHIS(); });

	ui.currentWidget->addSpecialButton(b);

}

void MedicalStatusDialog::setPastDiseases(const std::vector<std::string>& pd)
{
	ui.pastWidget->setMedicalStatus(pd);
}

std::vector<std::string> MedicalStatusDialog::getPastDiseases()
{
	return ui.pastWidget->getMedicalStatus();
}

void MedicalStatusDialog::setCurrentDiseases(const std::vector<std::string>& cd)
{
	ui.currentWidget->setMedicalStatus(cd);
}

std::vector<std::string> MedicalStatusDialog::getCurrentDiseases()
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

void MedicalStatusDialog::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	p.fillRect(rect(), Qt::white);
}
