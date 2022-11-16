#include "PrescriptionPresenter.h"
#include "MedicationPresenter.h"
#include "Database/DbPrescription.h"
#include "Model/FreeFunctions.h"

PrescriptionPresenter::PrescriptionPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId) :
	TabInstance(tabView, TabType::Prescription, patient), 
	view(tabView->perscriptionView()),
	m_prescription(DbPrescription::get(rowId)),
	patient_presenter(view->patientTile(), patient)
{
	m_prescription.patient_rowid = patient->rowid;

	if (!m_prescription.rowid) {
		m_prescription.LRN = FreeFn::getUuid();
		m_prescription.date = Date::currentDate();
	}
}

void PrescriptionPresenter::sendPrescriptionToHis()
{
	if (!m_prescription.NRN.empty()) return;

	if (!save()) return;

	issue_prescription.sendRequest(m_prescription, *patient, 
		[&](const std::string& nrn){
	
			if (nrn.empty()) return;

			m_prescription.NRN = nrn;

			makeEdited();
			save();
			
			if (!isCurrent()) return;
			
			view->setReadOnly(true);
			view->setNrn(nrn);
	});
}

void PrescriptionPresenter::cancelPrescription()
{
	if (m_prescription.NRN.empty()) return;

	cancel_prescription.sendRequest(m_prescription.NRN, [&](bool success) {
			
			if (success) {
				m_prescription.NRN = "";
				makeEdited();
				save();
				//if is current!
				view->setReadOnly(false);
				view->setNrn("");
				ModalDialogBuilder::showMessage("Рецептата е анулирана успешно");
			}
		}
	);
}

void PrescriptionPresenter::nrnButtonClicked()
{
	m_prescription.NRN.empty() ?
		sendPrescriptionToHis()
		:
		cancelPrescription();
}

void PrescriptionPresenter::addPressed()
{

	MedicationPresenter p;

	auto result = p.openDialog();

	if (!result) return;

	m_prescription.medicationGroup.push_back(result.value());
	
	view->setMedicationList(m_prescription.getMedList());

	makeEdited();
}

void PrescriptionPresenter::editPressed(int idx)
{
	if (idx == -1) return;

	MedicationPresenter p(m_prescription.medicationGroup[idx]);

	auto result = p.openDialog();

	if (!result) return;

	m_prescription.medicationGroup[idx] = result.value();

	view->setMedicationList(m_prescription.getMedList());

	makeEdited();

}

void PrescriptionPresenter::deletePressed(int idx)
{
	if (idx == -1) return;

	auto& medVec = m_prescription.medicationGroup;

	medVec.erase(medVec.begin() + idx);

	view->setMedicationList(m_prescription.getMedList());

	makeEdited();
}

void PrescriptionPresenter::supplementsChanged(const std::string& s)
{
	m_prescription.supplements = s;

	makeEdited();
}

void PrescriptionPresenter::dispensationChanged(const Dispensation& d)
{
	m_prescription.dispensation = d; 
	makeEdited();
}

void PrescriptionPresenter::dateChanged(const Date& date)
{
	m_prescription.date = date;
	patient_presenter.setDate(date);
	makeEdited();
}

void PrescriptionPresenter::setDataToView()
{
	view->setPresenter(this);
	patient_presenter.setCurrent();
	view->setDate(m_prescription.date);
	view->setDispensation(m_prescription.dispensation);
	view->setSupplements(m_prescription.supplements);
	view->setMedicationList(m_prescription.getMedList());
	view->setReadOnly(!m_prescription.NRN.empty());
	view->setNrn(m_prescription.NRN);

}

long long PrescriptionPresenter::rowID() const
{
	return m_prescription.rowid;
}

bool PrescriptionPresenter::save()
{

	if (!requiresSaving()) return true;

	if (m_prescription.medicationGroup.empty()) {
		ModalDialogBuilder::showError("Рецептата трябва да съдържа поне едно лекарство");
		return false;
	}

	if (isNew()) {
		m_prescription.rowid = DbPrescription::insert(m_prescription);
	}
	else
	{
		DbPrescription::update(m_prescription);
	}

	edited = false;

	refreshTabName();

	return true;
}

void PrescriptionPresenter::print() {}

bool PrescriptionPresenter::isNew()
{
	return m_prescription.rowid == 0;
}

TabName PrescriptionPresenter::getTabName()
{
	TabName result;
	result.header = isNew() ? "Нова рецепта " : "Рецепта ";

	result.header += m_prescription.date.toBgStandard();

	result.footer = patient->firstLastName();

	result.his = !m_prescription.NRN.empty();

	result.nhif = false;

	return result;
}
