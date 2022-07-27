#pragma once
#include "View/ReportDialog/IReportDialog.h"

#include <vector>
#include <unordered_map>
#include "Model/AmbList.h"
#include "Model/Patient.h"
#include "Network/NetService/DentalActivitiesService.h"
#include "Network/NetService/NraStatusService.h"
#include "Network/NetService/SendFileService.h"
#include <optional>
#include <queue>

class ReportDialogPresenter
{
	IReportDialog* view;

	//stores the amblists
	std::vector<AmbList> lists;

	int m_currentIndex{ -1 };
	bool m_hasErrors{ false };
	bool pisCheck{ false };
	bool nraCheck{ false };

	int year{ 0 };
	int month{ 0 };

	std::optional<std::string> m_report;

	//stores the rowid and the patient
	static inline std::unordered_map<long long, Patient> patients;

	DentalActivitiesService activitiesService;
	NraStatusService nraService;
	SendFileService sendFileService;

	//void pisCheckNext();
	void updateProgressBar();
	bool checkAmbList(const AmbList& list, const Patient& patient);
	void finish();

	void checkNext();

public:
	void reset();
	void sendToPis();
	void saveToXML();
	void setPISActivities(const std::optional<Procedures>& pisProcedures);
	void setInsuranceStatus(const std::optional<InsuranceStatus>& insuranceStatus);
	void setDate(int month, int year);
	void generateReport(bool checkPis, bool checkNra);
	void setView(IReportDialog* view);
};

