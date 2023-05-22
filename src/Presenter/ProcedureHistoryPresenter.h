#pragma once
#include "Model/Patient.h"
#include "Model/Dental/ToothContainer.h"
#include "Network/PIS/DentalActivitiesService.h"
#include "Network/HIS/EDental.h"
#include "View/Interfaces/IProcedureHistoryDialog.h"

class ProcedureHistoryPresenter
{
	DentalActivitiesService pis_service;
	EDental::GetProcedures his_service;
	EDental::GetStatus status_service;

	std::optional<std::vector<Procedure>> pis_history;
	std::optional<std::vector<Procedure>> his_history;
	std::optional<ToothContainer> current_status;

	bool m_applyPis{ false };
	bool m_applyStatus{ false };

	

	IProcedureHistoryDialog* view{ nullptr };

	const Patient& ref_patient;

public:

	struct Result {
		std::optional<std::vector<Procedure>>& pis_history;
		std::optional<std::vector<Procedure>>& his_history;
		std::optional<ToothContainer>& current_status;
		bool applyPis{ false };
		bool applyCurrentStatus{ false };
	};

	ProcedureHistoryPresenter(const Patient& p);

	void refreshPIS();
	void refreshHIS();
	void setView(IProcedureHistoryDialog* view);
	void openDialog();
	void pisApplyClicked();
	void statusApplyClicked();

	ProcedureHistoryPresenter::Result result();
};