#include "MedicationDialog.h"
#include "Model/Prescription/Medication.h"


MedicationDialog::MedicationDialog(MedicationPresenter* p, QWidget* parent)
	: presenter(p), QDialog(parent)
{
	ui.setupUi(this);

	setWindowTitle("Медикамент");

	ui.medicationEdit->setCompletions(Medication::names());
	ui.medicationEdit->setInputValidator(&nameValidator);

	connect(ui.quantity, &QSpinBox::valueChanged, [&] { commonDataChanged(); });
	connect(ui.quantityValue, &QComboBox::currentIndexChanged, [&] { commonDataChanged(); });
	connect(ui.substitutionCheck, &QCheckBox::stateChanged, [&](bool state) { commonDataChanged(); });
	connect(ui.priorityCombo, &QComboBox::currentIndexChanged, [=](int index) { presenter->priorityChanged(index); });
	connect(ui.notesEdit, &QLineEdit::textChanged, [=](const QString& text) { presenter->noteChanged(text.toStdString());});

	connect(ui.medicationEdit, &QLineEdit::textChanged, [=](const QString& text) { 
			presenter->medicationChanged(text.toStdString());
		});

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

	QSignalBlocker b[3]{ 
		QSignalBlocker{ui.substitutionCheck}, 
		QSignalBlocker{ui.quantity},
		QSignalBlocker{ui.priorityCombo}
	};

	ui.priorityCombo->setCurrentIndex(m.priority);
	ui.substitutionCheck->setChecked(m.substitution);
	ui.quantity->setValue(m.quantity);
	ui.quantityValue->setCurrentIndex(static_cast<int>(m.byForm));

	ui.notesEdit->setText(m.note.c_str());
}


MedicationDialog::~MedicationDialog()
{}

