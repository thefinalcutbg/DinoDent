#include "DosagePresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Prescription/Medication.h"

DosagePresenter::DosagePresenter(int medicationFormUnit)
{
	m_dosage.doseQuantity.setUnitFromCL010(Medication::getFormByKey(medicationFormUnit));
}

void DosagePresenter::setView(DosageDialog* view)
{
	this->view = view;

	view->setDosage(m_dosage);
}

void DosagePresenter::okPressed()
{
	if(view->fieldsAreValid()) {
		ok_pressed = true;
		view->close();
	}
}

std::optional<Dosage> DosagePresenter::openDialog()
{
    DosageDialog d(*this);

	d.exec();

	if (ok_pressed) return m_dosage;

	return std::optional<Dosage>{};
}
