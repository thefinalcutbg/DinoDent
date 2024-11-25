#include "MedicationDialog.h"
#include "Model/Prescription/Medication.h"

MedicationDialog::MedicationDialog(MedicationPresenter& p, QWidget* parent)
    : QDialog(parent), presenter(p)
{
	ui.setupUi(this);

	setWindowTitle("Медикамент");

	ui.medicationEdit->setCompletions(Medication::names());
	ui.medicationEdit->setInputValidator(&nameValidator);

	ui.formLabel->setText("*името на медикамента се изписва на латиница");

	ui.templateButton->setIcon(QIcon(":/icons/icon_template.png"));
	ui.deleteButton->setDisabled(true);
	ui.editButton->setDisabled(true);

    connect(ui.quantity, &QSpinBox::valueChanged, this, [&] { commonDataChanged(); });
    connect(ui.quantityValue, &QComboBox::currentIndexChanged, this, [&] { commonDataChanged(); });
    connect(ui.substitutionCheck, &QCheckBox::stateChanged, [&]{ commonDataChanged(); });
    connect(ui.priorityCombo, &QComboBox::currentIndexChanged, this, [&](int index) { presenter.priorityChanged(index); });
    connect(ui.notesEdit, &QLineEdit::textChanged, this, [&](const QString& text) { presenter.noteChanged(text.toStdString());});

    connect(ui.medicationEdit, &QLineEdit::textChanged, this, [&](const QString& text) {
            presenter.medicationChanged(text.toStdString());
		});


    connect(ui.periodGroup, &QGroupBox::clicked, this, [&]{ periodChanged(); });
    connect(ui.fromDate, &QDateEdit::dateChanged, this, [&] { periodChanged(); });
    connect(ui.toDate, &QDateEdit::dateChanged, this, [&] { periodChanged(); });
	connect(ui.templateButton, &QPushButton::clicked, this, [&] {presenter.addAsTemplate(); });
    connect(ui.addButton, &QPushButton::clicked, this, [&] {presenter.addDosage();});
    connect(ui.editButton, &QPushButton::clicked, this, [&] {presenter.editDosage(ui.dosageList->currentRow());});
    connect(ui.dosageList, &QListWidget::doubleClicked, this, [&] {ui.editButton->click();});
    connect(ui.deleteButton, &QPushButton::clicked, this, [&] { presenter.deleteDosage(ui.dosageList->currentRow());});
    connect(ui.okButton, &QPushButton::clicked, this, [&] {presenter.okPressed();});
    connect(ui.cancelButton, &QPushButton::clicked, this, [&] {close();});
    connect(ui.dosageList, &QListWidget::itemSelectionChanged, this, [&] {

		bool noSelection = ui.dosageList->selectedItems().empty();
		ui.editButton->setDisabled(noSelection);
		ui.deleteButton->setDisabled(noSelection);
		});
    presenter.setView(this);
}

void MedicationDialog::setQuantityListNames(const std::string& pack, const std::string& form)
{
	ui.quantityValue->setItemText(0, pack.c_str());
	ui.quantityValue->setItemText(1, form.c_str());
}

void MedicationDialog::setFormLabel(const std::string& formName)
{
	if (formName.empty()) {
		ui.formLabel->setText("Невалиден медикамент");
		return;
	}

	ui.formLabel->setText("Лекарствена форма: " + QString(formName.c_str()));
}

void MedicationDialog::setDosageList(const std::vector<std::string> dosageList)
{
	ui.dosageList->clear();

    for (auto& dosage : dosageList) {
		ui.dosageList->addItem(dosage.c_str());
	}

	ui.dosageList->setCurrentRow(ui.dosageList->count()-1);
}

void MedicationDialog::setReadOnly()
{
	QList<QWidget*> widgets = this->findChildren<QWidget*>();

	foreach(QWidget *widget, widgets)
	{
		auto label = qobject_cast<QLabel*>(widget);

		if (label) continue;

		widget->setEnabled(false);
	}
	ui.templateButton->setEnabled(true);
	ui.cancelButton->setEnabled(true);
	ui.okButton->setEnabled(true);
}

bool MedicationDialog::fieldsAreValid()
{
	if (!ui.medicationEdit->validateInput()) {
		ui.medicationEdit->setFocus();
		return false;
	}
	
	return true;
}


void MedicationDialog::commonDataChanged()
{
    presenter.commonDataChanged(
		ui.quantity->value(),
		ui.quantityValue->currentIndex(),
		ui.substitutionCheck->isChecked()
	);

}

void MedicationDialog::periodChanged()
{
    auto checked = ui.periodGroup->isChecked();

    if (!checked) {
        presenter.dosePeriodChanged({});
        return;
    };

    presenter.dosePeriodChanged(
        DosePeriod{
            .from = ui.fromDate->getDate(),
            .to = ui.toDate->getDate(),
        }
    );
}

void MedicationDialog::setMedication(const Medication & m)
{
	ui.medicationEdit->setText(m.name().c_str());

	ui.medicationEdit->setValidAppearence(true);

	QSignalBlocker b[6]{ 
		QSignalBlocker{ui.substitutionCheck}, 
		QSignalBlocker{ui.quantity},
		QSignalBlocker{ui.priorityCombo},
		QSignalBlocker{ui.periodGroup},
		QSignalBlocker{ui.fromDate},
		QSignalBlocker{ui.toDate}
	};

	ui.priorityCombo->setCurrentIndex(m.priority);
	ui.substitutionCheck->setChecked(m.substitution);
	ui.quantity->setValue(m.quantity);
	ui.quantityValue->setCurrentIndex(static_cast<int>(m.byForm));
	ui.notesEdit->setText(m.note.c_str());

	ui.periodGroup->setChecked(m.dosePeriod.has_value());

	if (m.dosePeriod)
	{
		ui.fromDate->set_Date(m.dosePeriod->from);
		ui.toDate->set_Date(m.dosePeriod->to);
	}
}


MedicationDialog::~MedicationDialog()
{}

