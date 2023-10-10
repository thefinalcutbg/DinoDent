#pragma once

#include "Model/Patient.h"
#include "View/Widgets/MedicalStatusDialog.h"

class MedicalStatusPresenter {

	MedicalStatusDialog* view;

	Patient& patient;

	std::vector<Allergy> allergies;
	MedicalStatuses other_statuses;


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