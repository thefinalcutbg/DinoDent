#pragma once
#include "Model/Patient.h"
#include "Model/TableStructs.h"
#include <queue>
#include <set>
#include "Network/NRA/NraStatusService.h"
#include "Network/PIS/DentalActivitiesService.h"

class UnusedPackageView;

struct PackageRowData {
	long long rowid;
	std::string patientName;
	int age;
	std::string patientPhone;
	std::string lastVisit;
	std::string exam;
	int procedure_count;
	int procedure_max;
	std::string lowerDenture;
	std::string upperDenture;
	static inline constexpr int columnCount = 9;
	std::string color_indicator;
};

struct UnusedPackageSettings{
	Date excludeBefore;
	bool pisCheckEnabled{ false };
	bool nraCheckEnabled{ false };
	bool nhifCurrentDentistOnly{ false };
};

class UnusedPackagePresenter
{
	UnusedPackageView* view = nullptr;

	static inline std::queue<Patient> m_queue;
	static inline std::vector<PackageRowData> s_data;
	static inline double m_sum = 0;

	static double calculatePackageSum(const PackageRowData& package);

	bool m_in_progress{ false };
	int m_year = Date::currentDate().year;

	static UnusedPackageSettings s_settings;

	ProcedureSummary currentProcedures;

	const std::set<int> code_set = { 301, 332, 333, 508, 509 };

	NraStatusService nraService;
	DentalActivitiesService pisService;
	
	void popQueue();

public:
	UnusedPackagePresenter();
	void setView(UnusedPackageView* view);
	void buttonPressed(const UnusedPackageSettings& settings);
	void stop(const std::string& reason = "");
	void resetQueue();
	void newAmbList(long long patientRowid);
	void step1_localDbCheck();
	void step2_insuranceCheck(const std::optional<InsuranceStatus>& status);
	void step3_pisCheck(const std::optional<std::vector<Procedure>>& pisHistory);
};