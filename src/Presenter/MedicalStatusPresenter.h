#pragma once

#include "Model/Patient.h"
#include "View/Widgets/MedicalStatusDialog.h"
#include "Network/HIS/EAllergy.h"

class MedicalStatusPresenter {

	MedicalStatusDialog* view{ nullptr };

	Patient& patient;

	EAllergy::Report report_service;
	EAllergy::Edit edit_service;
	EAllergy::Fetch fetch_service;

public:
	MedicalStatusPresenter(Patient& p);

	void loadAllergiesFromHis();

	void addAllergy();
	void removeAllergy(int idx);
	void editAllergy(int idx);
	void sendToHis(int idx);

	void okPressed();

	void openDialog();

};