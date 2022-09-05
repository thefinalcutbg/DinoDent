#include "MedicationDialog.h"
#include "Model/Perscription/Medication.h"

MedicationDialog::MedicationDialog(MedicationPresenter* p, QWidget *parent)
	: presenter(p), QDialog(parent)
{
	ui.setupUi(this);

	ui.medicationEdit->setCompletions(Medication::names());
	ui.medicationEdit->setInputValidator(&nameValidator);

	connect(ui.quantity, &QSpinBox::valueChanged, [=](int value) { presenter->quantityChanged(value); });
	connect(ui.medicationEdit, &QLineEdit::textChanged, [=](const QString& text) { 
			presenter->medicationChanged(text.toStdString());
		});

	connect(ui.substitutionCheck, &QCheckBox::stateChanged, [=](bool state) { presenter->substitutionChanged(state);});
	connect(ui.addButton, &QPushButton::clicked, [=] {presenter->addDosage();});
	presenter->setView(this);
}

void MedicationDialog::setQuantityListNames(const std::string& pack, const std::string& form)
{
	ui.quantityValue->setItemText(0, pack.c_str());
	ui.quantityValue->setItemText(1, form.c_str());
}

void MedicationDialog::setFormLabel(const std::string& formName)
{
	ui.formLabel->setText(u8"Лекарствена форма: " + QString(formName.c_str()));
}

MedicationDialog::~MedicationDialog()
{}
