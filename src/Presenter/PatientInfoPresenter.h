#pragma once
#include <memory>

#include "Model/Patient.h"
#include "Network/NRA/NraStatusService.h"
#include "Network/PIS/DiagnosisService.h"
#include "View/Interfaces/IPatientTileInfo.h"

class PatientInfoPresenter
{
	std::shared_ptr<Patient> patient;
	IPatientTileInfo* view;

	Date current_date{ Date::currentDate() };

	NraStatusService nraStatusServ;
	DiagnosisService nhifDiagnosisServ;

	bool m_showInsuranceDialog{ false };

	void setInsuranceStatus(const std::optional<InsuranceStatus>& status_result);


public:
	PatientInfoPresenter(IPatientTileInfo* view, std::shared_ptr<Patient> p);
	void setDate(const Date& date);
	void nraClicked(bool showDialog);
	void diagnosisClicked();
	void patientTileClicked();
	void allergiesTileClicked();
	void printDeclarations();
	void setCurrent();


};