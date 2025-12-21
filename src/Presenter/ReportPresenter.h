#pragma once

#include "Model/TableRows.h"
#include <vector>
#include <unordered_map>
#include <optional>

#include "Model/Dental/AmbList.h"
#include "Model/Dental/NhifSpecReport.h"
#include "Model/Patient.h"

#include "Network/BulkRequester.h"
#include "Network/PIS/SendFileService.h"


class ReportView;

class ReportPresenter
{
	ReportView* view{ nullptr };

	//stores the amblists
	std::vector<AmbList> lists;

    //points to sheet rowid for link activation
    std::map<long long, RowInstance> errorSheets;

    //prevents multiple exceeded daily limit messages
    std::set<std::string> exceededDailyLimitSet;


	int m_currentIndex{ -1 };
	bool m_hasErrors{ false };
	bool pisCheck{ false };
	bool nraCheck{ false };
    bool hisCheck{ false };

	int year{ 0 };
	int month{ 0 };

	std::optional<std::string> m_report;


	//stores the rowid and the patient
	static inline std::unordered_map<long long, Patient> patients;

    BulkRequester bulk_requester;
	SendFileService sendFileService;

	void updateProgressBar();
	void checkAmbList(const AmbList& list, const Patient& patient);
	void finish();
    void resultRecieved(const BulkRequester::Result& r);
	void checkNext();

public:
	
	void reset();
	void sendToPis();
	void saveToXML();
	void setDate(int month, int year);
    void generateReport(bool checkPis, bool checkNra, bool checkHis);
	void generateSpecification();
	void setView(ReportView* view);
	void linkClicked(long long sheetRowid);
};

