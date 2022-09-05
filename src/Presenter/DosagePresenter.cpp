#include "DosagePresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Model/Perscription/Medication.h"

DosagePresenter::DosagePresenter(int medicationFormUnit)
{
	m_dosage.doseQuantity.setUnitFromCL010(Medication::getFormByKey(medicationFormUnit));
}

void DosagePresenter::setView(IDosageDialog* view)
{
	this->view = view;

	view->setRouteList(Route::getStringList());	

	periodValueChanged(m_dosage.period.value);
	durationValueChanged(m_dosage.duration.value);
	frequencyChanged(m_dosage.frequency);
	doseQuantityValueChanged(m_dosage.doseQuantity.value);

}

std::optional<Dosage> DosagePresenter::openDialog()
{
	ModalDialogBuilder::openDialog(this);

	return std::optional<Dosage>();
}
