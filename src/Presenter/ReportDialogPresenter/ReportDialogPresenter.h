#pragma once
#include "View/ReportDialog/IReportDialog.h"

#include <vector>
#include <unordered_map>
#include "Model/AmbList.h"
#include "Model/Patient.h"
#include "Network/ReplyHandlers/DentalActivitiesHandler.h"
#include "Network/ReplyHandlers/NraReplyHandler.h"
#include "Network/ReplyHandlers/FileSentHandler.h"
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

	DentalActivitiesHandler<ReportDialogPresenter> pis_handler{this};
	NraReplyHandler<ReportDialogPresenter> nra_handler{ this };

	FileSentHandler fileSent_handler;

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

