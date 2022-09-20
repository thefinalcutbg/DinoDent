#include "DosagePresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Prescription/Medication.h"

DosagePresenter::DosagePresenter(int medicationFormUnit)
{
	m_dosage.doseQuantity.setUnitFromCL010(Medication::getFormByKey(medicationFormUnit));
}

void DosagePresenter::setView(IDosageDialog* view)
{
	this->view = view;

	view->setDosage(m_dosage);
}

void DosagePresenter::okPressed()
{
	if(view->fieldsAreValid()) {
		ok_pressed = true;
		view->closeUi();
	}
}

std::optional<Dosage> DosagePresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);

	if (ok_pressed) return m_dosage;

	return std::optional<Dosage>{};
}
