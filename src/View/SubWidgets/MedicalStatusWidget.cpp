#include "MedicalStatusWidget.h"
#include "View/ModalDialogBuilder.h"

MedicalStatusWidget::MedicalStatusWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
	connect(ui.addButton, &QPushButton::clicked, [this] {
			
			auto result = ModalDialogBuilder::inputDialog("", "Въвеждане на медицински статус");
			
			if (result.empty()) return;

			ui.statusList->addItem(result.c_str());
		}
	);

	connect(ui.removeButton, &QPushButton::clicked, [this] {

			auto index = ui.statusList->currentIndex();
			
			if (index.row() == -1) return;

			delete ui.statusList->takeItem(index.row());
		}
	);
	
	connect(ui.editButton, &QPushButton::clicked, [=] {

		auto index = ui.statusList->selectionModel()->currentIndex().row();

		if (index == -1) return;

		auto result = ModalDialogBuilder::inputDialog("", "Редакция", ui.statusList->item(index)->text().toStdString());

		if (result.size()) {
			ui.statusList->item(index)->setText(result.c_str());
		
		}
	});

	connect(ui.statusList, &QListWidget::doubleClicked, [=] { ui.editButton->click(); });
	
}

void MedicalStatusWidget::setMedicalStatus(const std::vector<MedicalStatus>& s)
{
	for (auto& status : s)
	{
		ui.statusList->addItem(status.data.c_str());

		//auto item = ui.statusList->item(ui.statusList->count() - 1);

		//item->setFlags(item->flags() | Qt::ItemIsEditable);
	}
}

std::vector<MedicalStatus> MedicalStatusWidget::getMedicalStatus()
{
	std::vector<MedicalStatus> result;

	for (int i = 0; i < ui.statusList->count(); i++)
	{

		result.push_back(
			{ 
				.nrn = {}, 
				.data =  ui.statusList->item(i)->text().toStdString() 
			}
		);
	}

	return result;
}

void MedicalStatusWidget::setName(const QString& name)
{
	ui.groupBox->setTitle(name);
}

MedicalStatusWidget::~MedicalStatusWidget()
{}
