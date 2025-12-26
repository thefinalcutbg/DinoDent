#pragma once

#include "View/Widgets/PatientHistoryDialog.h"
#include "Model/Patient.h"
#include "Network/PIS/DentalActivitiesService.h"
#include "Network/HIS/EDental.h"
#include "Network/HIS/EHospitalization.h"
#include "Model/TableRows.h"
#include <variant>
#include <vector>

class PatientHistoryPresenter {
	
	Patient& patient;

	std::vector<Procedure> local_history;

	std::vector<Hospitalization> hospitalizations;

	std::vector<RowInstance> documents;

	std::optional<std::vector<HisSnapshot>> his_snapshots;
	std::vector<HisSnapshot> local_snapshots;
	std::optional<std::vector<Procedure>> HISHistory; //HIS changes dinamically so no need to cache it

	PatientHistoryDialog view;

	DentalActivitiesService pis_service;
	EDental::GetStatusAndProcedures his_service;
	EHospitalization::Fetch eHospitalizationFetch;

	std::variant<std::monostate, std::vector<Procedure>*, ToothContainer> result;

public:
	PatientHistoryPresenter(Patient& patient); //updates the history of the patient directly

	void procedureSourceChanged(Procedure::DatabaseSource source);
	void procedureRefreshRequested(Procedure::DatabaseSource source);

	void statusRefreshRequested();
	void statusSourceChanged(Procedure::DatabaseSource source);
	void toothHistoryRequested(int toothIdx, Procedure::DatabaseSource source);
	void hospitalizationRequested();

	void openDocuments(const std::vector<int>& selectedDocIdx);
	void applyPISprocedures();
	void applyStatus(const ToothContainer& selectedStatus);

    decltype(result) openDialog(bool openWithLocal = false, bool forceShowApplyToStatus = false); //should return result

};
