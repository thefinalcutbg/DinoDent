#pragma once
#include "View/ReportDialog/IReportDialog.h"

#include <vector>
#include <unordered_map>
#include "Model/AmbList.h"
#include "Model/Patient.h"
#include "Network/ReplyHandlers/DentalActivitiesHandler.h"
#include <optional>
class ReportDialogPresenter
{
	IReportDialog* view;

	//stores the amblists
	std::vector<AmbList> lists;

	int m_currentIndex{ -1 };
	bool m_hasErrors{ false };

	int year{ 0 };
	int month{ 0 };

	std::optional<std::string> m_report;

	//stores the rowid and the patient
	static inline std::unordered_map<long long, Patient> patients;

	DentalActivitiesHandler<ReportDialogPresenter> reply_handler{this};

	void pisCheckNext();
	void updateProgressBar();
	bool checkAmbList(const AmbList& list, const Patient& patient);
	void finish();

public:
	void reset();
	void sendToPis();
	void saveToXML();
	void setPISActivities(const std::optional<Procedures>& pisProcedures);
	void setDate(int month, int year);
	void generateReport(bool checkPis);
	void setView(IReportDialog* view);
};

