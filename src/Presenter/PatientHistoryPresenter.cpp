#include "PatientHistoryPresenter.h"

#include "Database/DbProcedure.h"
#include "Database/DbInvoice.h"
#include "Database/DbPrescription.h"
#include "Database/DbBrowser.h"
#include "Database/DbPatient.h"
#include "Database/DbPatientSummary.h"
#include "Database/DbTreatmentPlan.h"

#include "View/ModalDialogBuilder.h"

#include "Model/User.h"

#include "Presenter/TabPresenter.h"
#include "Presenter/DetailedStatusPresenter.h"

PatientHistoryPresenter::PatientHistoryPresenter(Patient& patient) :
	patient(patient),
	local_history(DbProcedure::getPatientProcedures(patient.rowid)),
	view(*this)

{
	patient.teethNotes = DbPatient::getToothNotes(patient.rowid);

	PlainTable docTable;

	std::tie(this->documents, docTable) = DbBrowser::getPatientDocuments(patient.rowid);
	
	std::vector<PlainTable> doc_details;

	for (size_t i = 0; i < documents.size(); i++)
	{
		auto docRowid = documents[i].rowID;

		switch (documents[i].type)
		{
			case TabType::AmbList: doc_details.emplace_back(DbProcedure::getProcedures(docRowid)); break;
			case TabType::Financial: doc_details.emplace_back(DbInvoice::getInvoice(docRowid).businessOperations); break;
			case TabType::Prescription: doc_details.emplace_back(DbPrescription::get(docRowid).medicationGroup); break;
			case TabType::TreatmentPlan: doc_details.emplace_back(DbTreatmentPlan::getProcedureSummary(docRowid)); break;
			default: doc_details.emplace_back(PlainTable{}); break;
		}
	}

	view.setDocuments(docTable, doc_details);

	//init shapshots:

	ToothContainer lastToothStatus;
	
	std::vector<PerioSnapshot> perioSnapshots;

	for (auto& frame : DbPatientSummary::getFrames(patient.rowid)) {
		
		if (frame.type == TimeFrameType::Perio) {

			perioSnapshots.push_back(PerioSnapshot{
				.perioStatus = frame.perioData,
				.toothStatus = lastToothStatus,
				.perioStatistic = PerioStatistic(frame.perioData, patient.getAge(frame.perioData.date))
			});

			continue;
		}

		if (frame.type == TimeFrameType::InitialAmb) {
			local_snapshots.emplace_back(frame.teeth, frame.date);
			local_snapshots.back().procedure_name = "Амбулаторен лист " + frame.number + " (изходен статус)";
			local_snapshots.back().financing = static_cast<FinancingSource>(99);
			lastToothStatus = local_snapshots.back().teeth;
			continue;
		}

		//TimeFrameType::Procedures
		for (auto& p : frame.procedures) {

			local_snapshots.emplace_back(p, local_snapshots.back().teeth);

			if (local_snapshots.back().affected_teeth.empty()) {
				local_snapshots.pop_back(); //because it changes nothing
				lastToothStatus = local_snapshots.back().teeth;
			}

		}
	}

	view.setPerioSnapshots(perioSnapshots);

	view.setPatientNoteFlags(patient.teethNotes);
}

void PatientHistoryPresenter::procedureSourceChanged(Procedure::DatabaseSource source)
{
	switch (source)
	{
		case Procedure::PIS:
			if (!patient.PISHistory) {
				procedureRefreshRequested(source);
				break;
			}

			view.setProcedures(patient.PISHistory.value(), source);
			break;

		case Procedure::HIS:
			if (!HISHistory) {
				procedureRefreshRequested(source);
				break;
			}

			view.setProcedures(HISHistory.value(), source);
			break;

		case Procedure::Local:
			view.setProcedures(local_history, source);
			break;
	}
}

void PatientHistoryPresenter::procedureRefreshRequested(Procedure::DatabaseSource source)
{
	view.setProcedures({}, source);

	switch (source)
	{
		case Procedure::PIS:
			pis_service.sendRequest(patient, true,
				[=](const std::optional<std::vector<Procedure>> pis_data)
				{
					if (!pis_data.has_value()) return;

					if (pis_data->empty()) {
						ModalDialogBuilder::showMessage("За този пациент не са открити данни в ПИС");
					}

					patient.PISHistory = pis_data;
					view.setProcedures(patient.PISHistory.value(), source);
				}
			);
			break;

		case Procedure::HIS:

			his_service.sendRequest(patient, true,
				[=](const std::optional<std::vector<Procedure>>& procedures, const std::vector<HisSnapshot>& snapshot)
				{
					if (!procedures.has_value()) return;

					if (procedures->empty()) {
						ModalDialogBuilder::showMessage("За този пациент не са открити данни в НЗИС");
					}

					patient.HISHistory = procedures;
					HISHistory = procedures;
					view.setProcedures(procedures.value(), source);

				}
			);
			break;
	}
}



void PatientHistoryPresenter::statusSourceChanged(Procedure::DatabaseSource source)
{
	switch (source)
	{
		case Procedure::Local:
		{
			if (local_snapshots.size()) {
				view.setSnapshots(local_snapshots, Procedure::Local);
				break;
			}

			break;
		}

		case Procedure::HIS: 

			if (!his_snapshots) {
				statusRefreshRequested();
				return;
			}

			view.setSnapshots(*his_snapshots, Procedure::HIS);

			break;
	}
}

void PatientHistoryPresenter::toothHistoryRequested(int toothIdx, Procedure::DatabaseSource source)
{
	if (toothIdx < 1 || toothIdx > 32) return;

	DetailedStatusPresenter d(toothIdx, patient);

	d.open(source);

	patient.teethNotes[toothIdx] = d.getNote();

	view.setPatientNoteFlags(patient.teethNotes);
}

void PatientHistoryPresenter::statusRefreshRequested()
{
	his_service.sendRequest(patient, true,
		[&](const std::optional<std::vector<Procedure>>& procedures, const std::vector<HisSnapshot>& snapshots) {

			if (!procedures) return;

			if (snapshots.empty()) {
				ModalDialogBuilder::showMessage(
					"За този пациент не са намерени данни в НЗИС"
				);
				return;
			}

			his_snapshots = snapshots;

			view.setSnapshots(snapshots, Procedure::HIS);
			HISHistory = procedures;
			patient.HISHistory = procedures;
		}
	);

}

void PatientHistoryPresenter::hospitalizationRequested()
{
	eHospitalizationFetch.sendRequest(patient,
		User::practice().rziCode,
		[&](auto& hList) {

			if (!hList) return;

			hospitalizations = hList.value();
			view.setHospitalizations(hospitalizations);

		}
	);
}

void PatientHistoryPresenter::openDocuments(const std::vector<int>& selectedDocIdx)
{
	if (selectedDocIdx.empty()) return;

	bool someNotOpened = false;

	for (int i = 0; i < selectedDocIdx.size(); i++) {

		auto idx = selectedDocIdx[i];

		if (!TabPresenter::get().open(
			documents[idx], 
			i == selectedDocIdx.size()-1)
			)
		{
			someNotOpened = true;
		};
	}

	if (someNotOpened) {
		ModalDialogBuilder::showMessage("Документът не може да бъде отворен, тъй като не е издаден от този потребител");
		return;
	}

	view.close();

}

void PatientHistoryPresenter::applyPISprocedures()
{
	if (!patient.PISHistory) return;

	result = &patient.PISHistory.value();

	view.close();
}

void PatientHistoryPresenter::applyStatus(const ToothContainer& selectedStatus)
{
	result = selectedStatus;

	view.close();
}

decltype(PatientHistoryPresenter::result) PatientHistoryPresenter::openDialog(bool openWithLocal, bool forceShowApplyToStatus)
{
	view.setWindowTitle(
		view.windowTitle() + " - " +
		patient.fullName().c_str()
	);
	
	bool hasNhifContract = User::hasNhifContract();

	view.hideNhif(!hasNhifContract);

	if (openWithLocal) {
        view.open(Procedure::Local, forceShowApplyToStatus);
		
	}
	else {
		view.open(
			hasNhifContract ?
			Procedure::PIS
			:
            Procedure::HIS,
            forceShowApplyToStatus
		);
	}
	
	return result;
}


