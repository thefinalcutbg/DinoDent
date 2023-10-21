#include "MedicationDialog.h"
#include "Model/Prescription/Medication.h"
#include "QtVersion.h"

MedicationDialog::MedicationDialog(MedicationPresenter* p, QWidget* parent)
	: presenter(p), QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Медикамент");

	ui.medicationEdit->setCompletions(Medication::names());
	ui.medicationEdit->setInputValidator(&nameValidator);

	connect(ui.quantity, QtSpinValueChanged, [&] { commonDataChanged(); });
	connect(ui.quantityValue, QtComboIndexChanged, [&] { commonDataChanged(); });
	connect(ui.substitutionCheck, &QCheckBox::stateChanged, [&](bool state) { commonDataChanged(); });
	connect(ui.priorityCombo, QtComboIndexChanged, [=](int index) { presenter->priorityChanged(index); });
	connect(ui.notesEdit, &QLineEdit::textChanged, [=](const QString& text) { presenter->noteChanged(text.toStdString());});

	connect(ui.medicationEdit, &QLineEdit::textChanged, [=](const QString& text) { 
			presenter->medicationChanged(text.toStdString());
		});

	auto dosePeriodGetter = [=] {

		auto checked = ui.periodGroup->isChecked();

		if (!checked) {
			presenter->dosePeriodChanged({});
			return;
		};

		presenter->dosePeriodChanged(
			DosePeriod{
				.from = ui.fromDate->getDate(),
				.to = ui.toDate->getDate(),
			}
		);
	};

	connect(ui.periodGroup, &QGroupBox::clicked, [=]{ dosePeriodGetter(); });
	connect(ui.fromDate, &QDateEdit::dateChanged, [=] { dosePeriodGetter(); });
	connect(ui.toDate, &QDateEdit::dateChanged, [=] { dosePeriodGetter(); });

	connect(ui.addButton, &QPushButton::clicked, [=] {presenter->addDosage();});
	connect(ui.editButton, &QPushButton::clicked, [=] {presenter->editDosage(ui.dosageList->currentRow());});
	connect(ui.dosageList, &QListWidget::doubleClicked, [=] {ui.editButton->click();});
	connect(ui.deleteButton, &QPushButton::clicked, [=] { presenter->deleteDosage(ui.dosageList->currentRow());});
	connect(ui.okButton, &QPushButton::clicked, [&] {presenter->okPressed();});
	connect(ui.cancelButton, &QPushButton::clicked, [&] {close();});
	
	presenter->setView(this);
}

void MedicationDialog::setQuantityListNames(const std::string& pack, const std::string& form)
{
	ui.quantityValue->setItemText(0, pack.c_str());
	ui.quantityValue->setItemText(1, form.c_str());
}

void MedicationDialog::setFormLabel(const std::string& formName)
{
	if (formName.empty()) {
		ui.formLabel->setText("");
		return;
	}

	ui.formLabel->setText("Лекарствена форма: " + QString(formName.c_str()));
}

void MedicationDialog::setDosageList(const std::vector<std::string> dosageList)
{
	auto dosageIdx = ui.dosageList->currentIndex();

	ui.dosageList->clear();

	for (auto dosage : dosageList) {
		ui.dosageList->addItem(dosage.c_str());
	}

	ui.dosageList->setCurrentIndex(dosageIdx);
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
	presenter->commonDataChanged(
		ui.quantity->value(),
		ui.quantityValue->currentIndex(),
		ui.substitutionCheck->isChecked()
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

