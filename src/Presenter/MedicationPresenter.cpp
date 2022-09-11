#include "MedicationPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "DosagePresenter.h"
#include <QDebug>
MedicationPresenter::MedicationPresenter()
{
}

void MedicationPresenter::commonDataChanged(int quantity, bool quantityByForm, bool allowSubstitution)
{
	m_medication.quantityValue = quantity;
	m_medication.isQuantityByForm = quantityByForm;
	m_medication.isSubstitutionAllowed = allowSubstitution;

	if (quantity == 1) {
		view->setQuantityListNames(u8"Опаковка", u8"Лекарствена форма");
	}
	else
	{
		view->setQuantityListNames(u8"Опаковки", u8"Лекарствени форми");
	}
}

void MedicationPresenter::medicationChanged(const std::string& mediName)
{
	if (m_medication.setName(mediName)) {

		view->setFormLabel(m_medication.formStr());
		return;
	}

	view->setFormLabel("");

}

void MedicationPresenter::noteChanged(const std::string& note)
{
	m_medication.note = note;
}

void MedicationPresenter::addDosage()
{
	if (!view->fieldsAreValid()){
		
		return;
	}

	DosagePresenter p(m_medication.form);
	auto result = p.openDialog();

	if (result.has_value()) {
		m_medication.dosage.push_back(result.value());
	}

	view->setDosageList(m_medication.dosageList());
}

std::optional<Medication> MedicationPresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);

	if (ok_pressed) return m_medication;

	return std::optional<Medication>();
}

void MedicationPresenter::editDosage(int idx)
{
	if (idx == -1) return;

	DosagePresenter p(m_medication.dosage[idx]);

	auto result = p.openDialog();

	if (result) m_medication.dosage[idx] = result.value();

	view->setDosageList(m_medication.dosageList());
}

void MedicationPresenter::deleteDosage(int idx)
{
	if (idx == -1) return;

	m_medication.dosage.erase(m_medication.dosage.begin() + idx);

	view->setDosageList(m_medication.dosageList());
}

void MedicationPresenter::okPressed()
{

	if (m_medication.dosage.empty())
	{
		ModalDialogBuilder::showMessage(u8"Добавете поне една дозировка!");
		return;
	}

	if (!view->fieldsAreValid()) return;

	ok_pressed = true;

	view->closeUi();
}

void MedicationPresenter::setView(IMedicationDialog* view)
{
	this->view = view;

	view->setMedication(m_medication);
	view->setDosageList(m_medication.dosageList());
}
