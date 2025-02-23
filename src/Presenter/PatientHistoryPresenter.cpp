#include "PatientHistoryPresenter.h"
#include "Database/DbProcedure.h"
#include "Database/DbInvoice.h"
#include "Database/DbPrescription.h"
#include "Database/DbBrowser.h"
#include "Database/DbPatientSummary.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User.h"
#include "Presenter/TabPresenter.h"

PatientHistoryPresenter::PatientHistoryPresenter(Patient& patient) :
	patient(patient),

	local_history(DbProcedure::getPatientProcedures(patient.rowid)),
	view(*this)

{
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
			case TabType::PerioStatus: doc_details.emplace_back(PlainTable{}); break;
		}
	}

	view.setDocuments(docTable, doc_details);

	//init shapshots:

	for (auto& frame : DbPatientSummary::getFrames(patient.rowid)) {

		if (frame.type == TimeFrameType::Perio) continue;

		if (frame.type == TimeFrameType::InitialAmb) {
			local_snapshots.emplace_back(frame.teeth, frame.date);
			local_snapshots.back().procedure_name = "Амбулаторен лист " + frame.number + " (изходен статус)";
			local_snapshots.back().financing = static_cast<FinancingSource>(99);
			continue;
		}

		//TimeFrameType::Procedures
		for (auto& p : frame.procedures) {

			local_snapshots.emplace_back(p, local_snapshots.back().teeth);

			if (local_snapshots.back().affected_teeth.empty()) {
				local_snapshots.pop_back(); //because it changes nothing
			}

		}
	}

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
			if (!patient.HISHistory) {
				procedureRefreshRequested(source);
				break;
			}

			view.setProcedures(patient.HISHistory.value(), source);
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
				[=](const std::optional<std::vector<Procedure>> his_data)
				{
					if (!his_data.has_value()) return;

					if (his_data->empty()) {
						ModalDialogBuilder::showMessage("За този пациент не са открити данни в НЗИС");
					}

					patient.HISHistory = his_data;
					view.setProcedures(patient.HISHistory.value(), source);

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

void PatientHistoryPresenter::statusRefreshRequested()
{
	dental_history_service.sendRequest(patient,
		[&](const std::vector<HisSnapshot>& snapshots) {

			if (snapshots.empty()) {
				ModalDialogBuilder::showMessage(
					"За този пациент не са намерени данни в НЗИС"
				);
				return;
			}

			his_snapshots = snapshots;

			view.setSnapshots(snapshots, Procedure::HIS);

		}
	);

}

void PatientHistoryPresenter::hospitalizationRequested()
{
	eHospitalizationFetch.sendRequest(patient,
		User::practice().rziCode,
		[&](const std::vector<Hospitalization>& hList) {

			hospitalizations = hList;
			view.setHospitalizations(hList);

		}
	);
}

void PatientHistoryPresenter::openDocuments(const std::vector<int>& selectedDocIdx)
{
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

decltype(PatientHistoryPresenter::result) PatientHistoryPresenter::openDialog(bool openWithLocal)
{
	view.setWindowTitle(
		view.windowTitle() + " - " +
		patient.fullName().c_str()
	);
	
	bool hasNhifContract = User::hasNhifContract();

	view.hideNhif(!hasNhifContract);

	if (openWithLocal) {
		view.open(Procedure::Local);
		
	}
	else {
		view.open(
			hasNhifContract ?
			Procedure::PIS
			:
			Procedure::HIS
		);
	}
	
	return result;
}


