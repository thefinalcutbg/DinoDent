#include "PrescriptionPresenter.h"
#include "MedicationPresenter.h"
#include "Database/DbPrescription.h"
#include "Model/FreeFunctions.h"
#include "Model/User.h"
#include "View/Printer.h"
#include "View/Widgets/MedicationTemplateDialog.h"
#include "GlobalSettings.h"
#include "Model/DirTree.h"

PrescriptionPresenter::PrescriptionPresenter(ITabView* tabView, std::shared_ptr<Patient> patient, long long rowId) :
	TabInstance(tabView, TabType::Prescription, patient), 
	view(tabView->prescriptionView()),
	m_prescription(DbPrescription::get(rowId)),
	patient_info(view->patientTile(), patient)
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

				ModalDialogBuilder::showMessage("Рецептата е анулирана успешно");
				
				if (isCurrent()) view->setNrn("");
			
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

void PrescriptionPresenter::checkStatus()
{
	if (m_prescription.NRN.empty()) return;

	check_status_service.sendRequest(m_prescription.NRN, [&](const std::string result) {

			ModalDialogBuilder::showMessage(result);
		}
	);
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
	if (idx < 0 || idx >= m_prescription.medicationGroup.size()) return;

	MedicationPresenter p(m_prescription.medicationGroup[idx], m_prescription.NRN.size());

	auto result = p.openDialog();

	if (!result) return;

	m_prescription.medicationGroup[idx] = result.value();

	view->setMedicationList(m_prescription.getMedList());

	makeEdited();

}

void PrescriptionPresenter::deletePressed(int idx)
{
	if (idx == -1 || m_prescription.NRN.size()) return;

	auto& medVec = m_prescription.medicationGroup;

	medVec.erase(medVec.begin() + idx);

	view->setMedicationList(m_prescription.getMedList());

	makeEdited();
}

void PrescriptionPresenter::eRxPressed()
{

	auto nrn = ModalDialogBuilder::inputDialog("НРН на рецептата:", "Зареждане на рецепта от eRx");

	if (nrn.empty()) return;

	if (DbPrescription::nrnExists(nrn)) {
		ModalDialogBuilder::showMessage("Рецепта с това НРН вече съществува в базата данни");
		return;
	}

	fetch_service.sendRequest(nrn, User::doctor().LPK, *patient,
		[&](const Prescription& prescr) {

			auto rowid = m_prescription.rowid;
			auto patient_rowid = m_prescription.patient_rowid;
			edited = true;

			m_prescription = prescr;
			m_prescription.rowid = rowid;
			m_prescription.patient_rowid = patient_rowid;

			save();

			refreshTabName();

			if (isCurrent()) setDataToView();
	});
}

void PrescriptionPresenter::addTemplate()
{
	MedicationTemplateDialog d;
	d.exec();

	auto result = d.getResult();

	if (!result) return;

	m_prescription.medicationGroup.push_back(result.value());

	view->setMedicationList(m_prescription.getMedList());

	makeEdited();
}

void PrescriptionPresenter::setFemaleProperties(bool pregnancy, bool breastfeeding)
{
	if (!patient->canBePregnant()) return;

	m_prescription.isPregnant = pregnancy;
	m_prescription.isBreastFeeding = breastfeeding;

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
	patient_info.setDate(date);

	view->setMisc(
		patient->canBePregnant(m_prescription.date),
		m_prescription.isPregnant,
		m_prescription.isBreastFeeding
	);

	makeEdited();
}

void PrescriptionPresenter::setDataToView()
{
	view->setPresenter(this);
	patient_info.setCurrent(true);

	view->setDate(m_prescription.date);
	view->setDispensation(m_prescription.dispensation);
	view->setSupplements(m_prescription.supplements);
	view->setMedicationList(m_prescription.getMedList());

	view->setNrn(m_prescription.NRN);
	view->setMisc(
		patient->canBePregnant(m_prescription.date),
		m_prescription.isPregnant,
		m_prescription.isBreastFeeding
	);


}

long long PrescriptionPresenter::rowID() const
{
	return m_prescription.rowid;
}

bool PrescriptionPresenter::save()
{

	if (!requiresSaving()) return true;

	if (m_prescription.medicationGroup.empty()) {
		ModalDialogBuilder::showError("Рецептата трябва да съдържа поне един медикамент");
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

void PrescriptionPresenter::print() 
{
	Print::prescription(m_prescription, *patient);
}

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

	if (m_prescription.NRN.size()) {
		result.header_icon = CommonIcon::HIS;
	}

	return result;
}

void PrescriptionPresenter::pdfPrint()
{
	if (m_prescription.NRN.empty()) {
		ModalDialogBuilder::showMessage(
			"Първо изпратете рецептата в НЗИС"
		);
		return;
	}

	auto filepath = DirTree::get(m_prescription, *patient);

	if (filepath.empty()) return;

	Print::prescription(m_prescription, *patient, filepath);
	
}
