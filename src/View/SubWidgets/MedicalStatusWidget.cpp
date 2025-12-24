#include "MedicalStatusWidget.h"
#include "View/ModalDialogBuilder.h"
#include "View/CommonIcon.h"

MedicalStatusWidget::MedicalStatusWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
    connect(ui.addButton, &QPushButton::clicked, this, [&] {
			
			auto result = ModalDialogBuilder::icdDialog(ICD10("A00"));
			
			if (!result.isValid()) return;

            m_status_list.push_back(MedicalStatus{result});

			ui.statusList->addItem(result.name().c_str());

            ui.statusList->setHidden(!ui.statusList->count());
		}
	);

	connect(ui.editButton, &QPushButton::clicked, this, [&] {

		auto index = ui.statusList->selectionModel()->currentIndex().row();

		if (index == -1) return;

		if (m_status_list[index].nrn.size()) {
			return;
		}

		auto result = ModalDialogBuilder::icdDialog(m_status_list[index].diagnosis.code());

		if (!result.isValid())  return;
		
		m_status_list[index].diagnosis = result;

		ui.statusList->item(index)->setText(result.name().c_str());
	});

    connect(ui.removeButton, &QPushButton::clicked, this, [&] {

			auto index = ui.statusList->currentIndex().row();
			
			if (index == -1) return;

			m_status_list.erase(m_status_list.begin() + index);

			setMedicalStatus(m_status_list);

            ui.statusList->setHidden(!ui.statusList->count());
		}
	);
	

    connect(ui.statusList, &QListWidget::doubleClicked, this, [&] { ui.editButton->click(); });

    connect(ui.statusList, &QListWidget::itemSelectionChanged, this, [&] {
			
		bool noSelection = ui.statusList->selectedItems().empty();
		ui.editButton->setDisabled(noSelection || !ui.statusList->selectedItems()[0]->icon().isNull());
		ui.removeButton->setDisabled(noSelection);
	});


    ui.statusList->itemSelectionChanged();
}

void MedicalStatusWidget::setMedicalStatus(const std::vector<MedicalStatus>& s)
{
	ui.statusList->clear();

	m_status_list = s;

	for (auto& status : s)
	{
		auto item = new QListWidgetItem(status.diagnosis.name().c_str());

		if (status.nrn.size()) {
			item->setIcon(CommonIcon::getPixmap(CommonIcon::HIS));
		}

        ui.statusList->addItem(item);
    }

    ui.statusList->setHidden(!ui.statusList->count());
}

std::vector<MedicalStatus> MedicalStatusWidget::getMedicalStatus()
{
	return m_status_list;
}

void MedicalStatusWidget::setName(const QString& name)
{
	ui.groupBox->setTitle(name);
}

void MedicalStatusWidget::addSpecialButton(QPushButton* b)
{
	ui.bottomLayout->addWidget(b, 0, Qt::AlignRight);
}

MedicalStatusWidget::~MedicalStatusWidget()
{}
