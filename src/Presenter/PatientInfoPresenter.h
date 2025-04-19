#pragma once
#include <memory>

#include "Model/Patient.h"
#include "Network/NRA/NraStatusService.h"
#include "Network/PIS/DiagnosisService.h"
#include "Network/HIS/EAllergy.h"
#include "Network/HIS/EHospitalization.h"
#include "View/Interfaces/IPatientTileInfo.h"
#include "Model/TabType.h"

struct PatientInfoPresenter
{
	std::shared_ptr<Patient> patient;
	IPatientTileInfo* view;

	int patientAge{ 0 };

	NraStatusService nraStatusServ;
	DiagnosisService nhifDiagnosisServ;
	EAllergy::Fetch allergyFetchServ;
	EHospitalization::Fetch eHospitalizationFetch;

	bool m_showInsuranceDialog{ false };

	void setInsuranceStatus(const std::optional<InsuranceStatus>& status_result);

	bool m_isCurrent{ false };

public:
	PatientInfoPresenter(IPatientTileInfo* view, std::shared_ptr<Patient> p);
	void setDate(const Date& date);
	void nraClicked(bool showDialog);
	void diagnosisClicked();
	void allergiesClicked();
	void patientTileClicked();
	void medStatTileClicked();
	void appointmentClicked();
	void notesRequested();
	void checkHospitalization();
	void setCurrent(bool current);
    void notificationClicked();
    void openDocument(TabType type);


};
