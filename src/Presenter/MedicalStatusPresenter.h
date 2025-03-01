#pragma once

#include "Model/Patient.h"
#include "View/Widgets/MedicalStatusDialog.h"
#include "Network/HIS/EAllergy.h"
#include "Network/PIS/DiagnosisService.h"
#include "Network/HIS/EClinicalCondition.h"

class MedicalStatusPresenter {

	MedicalStatusDialog* view{ nullptr };

	Patient& patient;

	EAllergy::Report report_service;
	EAllergy::Edit edit_service;
	EAllergy::Fetch fetch_service;

    EClinicalCondition::Fetch fetch_condition_service;
	DiagnosisService nhis_diag_service;

	bool awaitingReply();

public:
	MedicalStatusPresenter(Patient& p);

	void loadAllergiesFromHis();
	void loadICDFromNHIS();
    void loadICDFromHIS();

	void addAllergy();
	void removeAllergy(int idx);
	void editAllergy(int idx);
	void sendAllergyToHis(int idx);

	void openDialog();

};
