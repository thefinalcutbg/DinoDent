#pragma once
#include <vector>
#include <unordered_set>
#include "ProcedureTemplate.h"
#include "NZOKmaps.h"


class MasterNZOK
{
	static MasterNZOK _instance;

	std::unordered_map<int, ProcedureTemplate> _procedures;
	std::unordered_map<int, int> code_durations;
	std::vector<NZOKUpdates> _updates;

	std::unordered_set<int> minor_only;
	std::unordered_set<int> temp_only;
	std::unordered_set<int> perma_only;
	

	MasterNZOK();
	void loadData();

public:
	MasterNZOK(const MasterNZOK&) = delete;
	static MasterNZOK& instance();

	void loadUpdates();

	int getDuration(int nzokCode);
	std::vector<ProcedureTemplate> getM_Templates(Date date, int specialty, bool adult, bool unfav);
	std::pair<patientPrice, nzokPrice> getPrices(int code, Date date, int specialty, bool adult, bool unfav);
	double getPatientPrice(int code, Date date, int specialty, bool adult, bool unfav);
	double getNZOKPrice(int code, Date date, int specialty, bool adult, bool unfav);

	std::vector<ProcedurePackage> getPackages(Date ambDate);

	ProcedureTemplate getTemplateByCode(int code);

	bool isTempOnly(int code);
	bool isPermaOnly(int code);
	bool isMinorOnly(int code);
};

