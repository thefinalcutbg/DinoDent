#pragma once

#include "Model/Patient.h"
#include "View/Interfaces/IPatientDialog.h"
#include "Network/PIS/GetHirbnoService.h"

class PatientDialogPresenter
{
	std::optional<Patient> m_patient;

	IPatientDialog* view;

	std::string dialogTitle;

	GetHirbnoService hirbnoHandler;

	//data not present in view:
	std::optional<InsuranceStatus> insurance_status;
	std::optional<std::vector<Procedure>> PISHistory;
	std::optional<std::vector<Procedure>> HISHistory;
	long long rowid{ 0 };
	MedicalStatuses medStats;
	std::vector<Allergy> allergies;

	Patient getPatientFromView(); //sets the data not present in view

public:
	PatientDialogPresenter(std::string dialogTitle);
	PatientDialogPresenter(const Patient& patient);
	std::optional<Patient> open();

	void setHirbno(const std::string& hirbno);
	void checkHirbno();
	void searchDbForPatient(int type, const std::string& id);
	void accept();

	void setView(IPatientDialog* view);

};

