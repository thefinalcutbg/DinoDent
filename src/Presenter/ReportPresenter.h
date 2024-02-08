#pragma once
#include "View/Interfaces/IReportView.h"

#include "Model/TableRows.h"
#include <vector>
#include <unordered_map>
#include "Model/Dental/AmbList.h"
#include "Model/Dental/NhifSpecReport.h"
#include "Model/Patient.h"

#include "Network/PIS/DentalActivitiesService.h"
#include "Network/NRA/NraStatusService.h"
#include "Network/PIS/SendFileService.h"
#include <optional>
#include <queue>

class ReportPresenter
{
	IReportView* view{ nullptr };

	//stores the amblists
	std::vector<AmbList> lists;
	std::map<std::string, RowInstance> errorSheets;

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
	void checkAmbList(const AmbList& list, const Patient& patient);
	void finish();

	void checkNext();

public:
	
	void reset();
	void sendToPis();
	void saveToXML();
	void setPISActivities(const std::optional<std::vector<Procedure>>& pisProcedures);
	void setInsuranceStatus(const std::optional<InsuranceStatus>& insuranceStatus);
	void setDate(int month, int year);
	void generateReport(bool checkPis, bool checkNra);
	void generateSpecification();
	void setView(IReportView* view);
	void linkClicked(const std::string& sheetNumber);
};

