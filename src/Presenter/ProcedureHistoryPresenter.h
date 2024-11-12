#pragma once
#include "Model/Patient.h"
#include "Model/Dental/ToothContainer.h"
#include "Network/PIS/DentalActivitiesService.h"
#include "Network/HIS/EDental.h"
#include "Network/HIS/EHospitalization.h"

class ProcedureHistoryDialog;

class ProcedureHistoryPresenter
{

public:
	struct Result {
		std::optional<std::vector<Procedure>> pis_history;
		std::optional<std::vector<Procedure>> his_history;
		std::optional<ToothContainer> statusToBeApplied;
		bool applyPis{ false };
	};

private:
	DentalActivitiesService pis_service;
	EDental::GetProcedures his_service;
	EDental::GetStatus status_service;
	EDental::GetDentalHistory dental_history_service;
	EHospitalization::Fetch eHospitalizationFetch;

	std::optional<std::vector<Procedure>> pis_history;
	std::optional<std::vector<Procedure>> his_history;

	std::vector<Hospitalization> hospitalizations;

	//query to HIS or PIS is sent only when the tab is focused for first time
	bool tabIdxFirstFocus[3] = { true, true, true };

	bool hasHSM = true;

	ProcedureHistoryDialog* view{ nullptr };

	Result m_result;

	const Patient& ref_patient;

public:

	ProcedureHistoryPresenter(const Patient& p);

	bool refreshPIS();
	bool refreshHIS();
	bool refreshStatus();
	bool refreshHospitalizations();

	void setView(ProcedureHistoryDialog* view);
	void openDialog();
	void pisApplyClicked();
	void statusApplyClicked();
	void tabFocused(int idx);
	
	ProcedureHistoryPresenter::Result result();
};