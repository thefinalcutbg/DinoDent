#pragma once
#include "Model/Patient.h"
#include "Model/TableStructs.h"
#include <queue>
#include <set>
#include "Network/NRA/NraStatusService.h"
#include "Network/PIS/DentalActivitiesService.h"

class UnusedPackageView;

class UnusedPackagePresenter
{
	UnusedPackageView* view;

	static inline std::queue<Patient> m_queue;
	bool m_in_progress{ false };

	int m_year = Date::currentDate().year;

	ProcedureSummary currentProcedures;

	const std::set<int> code_set = { 301, 332, 333, 508, 509 };

	NraStatusService nraService;
	DentalActivitiesService pisService;

	void popQueue();
public:
	UnusedPackagePresenter(UnusedPackageView* view);
	void buttonPressed(const Date& date);
	void stop(const std::string& reason = "");
	void resetQueue();
	void newAmbList(long long patientRowid);
	void step1_localDbCheck();
	void step2_insuranceCheck(const std::optional<InsuranceStatus>& status);
	void step3_pisCheck(const std::optional<std::vector<Procedure>>& pisHistory);
};