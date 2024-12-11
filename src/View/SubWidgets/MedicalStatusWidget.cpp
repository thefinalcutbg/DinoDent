#include "MedicalStatusWidget.h"
#include "View/ModalDialogBuilder.h"

MedicalStatusWidget::MedicalStatusWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
    connect(ui.addButton, &QPushButton::clicked, this, [&] {
			
			auto result = ModalDialogBuilder::icdDialog(ICD10("A00"));
			
			if (!result.isValid()) return;

			m_icd_list.push_back(result);

			ui.statusList->addItem(result.name().c_str());
		}
	);

	connect(ui.editButton, &QPushButton::clicked, this, [&] {

		auto index = ui.statusList->selectionModel()->currentIndex().row();

		if (index == -1) return;

		auto result = ModalDialogBuilder::icdDialog(m_icd_list[index]);

		if (!result.isValid())  return;
		
		m_icd_list[index] = result;

		ui.statusList->item(index)->setText(result.name().c_str());
	});

    connect(ui.removeButton, &QPushButton::clicked, this, [&] {

			auto index = ui.statusList->currentIndex().row();
			
			if (index == -1) return;

			m_icd_list.erase(m_icd_list.begin() + index);

			setMedicalStatus(m_icd_list);
		}
	);
	

    connect(ui.statusList, &QListWidget::doubleClicked, this, [&] { ui.editButton->click(); });

    connect(ui.statusList, &QListWidget::itemSelectionChanged, this, [&] {
			
		bool noSelection = ui.statusList->selectedItems().empty();
		ui.editButton->setDisabled(noSelection);
		ui.removeButton->setDisabled(noSelection);
	});


    ui.statusList->itemSelectionChanged();
}

void MedicalStatusWidget::setMedicalStatus(const std::vector<ICD10>& s)
{
	ui.statusList->clear();

	m_icd_list = s;

	for (auto& status : s)
	{
		ui.statusList->addItem(status.name().c_str());
	}
}

std::vector<ICD10> MedicalStatusWidget::getMedicalStatus()
{
	return m_icd_list;
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
