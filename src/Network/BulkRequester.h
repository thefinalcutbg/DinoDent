#pragma once

#include "Network/HIS/EAllergy.h"
#include "Network/HIS/EDental.h"
#include "Network/HIS/EClinicalCondition.h"
#include "Network/HIS/EHospitalization.h"
#include "Network/PIS/DentalActivitiesService.h"
#include "Network/PIS/DiagnosisService.h"
#include "Network/NRA/NraStatusService.h"
#include "Network/NRA/NssiPentionService.h"
#include "Network/PIS/GetHirbnoService.h"

class BulkRequester
{
	DentalActivitiesService dentalActService;
	NraStatusService nraStatusServ;
	GetHirbnoService hirbnoService;
	DiagnosisService nhifDiagnosisServ;
	EAllergy::Fetch eAllergyFetchService;
	EClinicalCondition::Fetch eClinicalConditionFetchService;
	EHospitalization::Fetch eHospitalizationFetchService;
	EDental::GetStatusAndProcedures eDentalGetStatusAndProceduresService;


	Patient p;

	std::vector<std::function<bool()>> steps;

	void nextStep();

public:

	BulkRequester();

	struct Result {
		std::optional<std::vector<Procedure>> pisDentalActivities;
		std::optional<std::vector<Procedure>> hisDentalRecords;
		std::optional<InsuranceStatus> nraStatus;
		std::string hirbNo;
		std::vector<Allergy> allergies;
		std::vector<MedicalStatus> pastDiseases;
		std::vector<MedicalStatus> currentDiseases;
		std::vector<Hospitalization> hospitalizations;
		std::vector<HisSnapshot> hisSnapshots;
	};

	enum RequestType {
		NhifProcedures,
		HISProcedures,
		NraStatus,
		HirbNo,
		NhifMedicalConditions,
		HISMedicalConditions,
		Allergies,
		Hospitalizations
	};

	void sendRequest(
		const Patient& patient,
		const std::vector<RequestType>& requestTypes
	);

	void setCallback(std::function<void(const Result&)> callback) {
		m_callback = callback;
	}
	
	bool awaitingReply() {
		return steps.size() > 0;
	}

private:
	std::function<void(const Result&)> m_callback;

	Result m_result;

};