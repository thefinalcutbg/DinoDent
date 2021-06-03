#pragma once
#include <vector>
#include <unordered_set>
#include "ManipulationTemplate.h"
#include "NZOKmaps.h"



class MasterNZOK
{
	static MasterNZOK _instance;

	std::unordered_map<int, ManipulationTemplate> _manipulations;
	std::unordered_map<int, int> code_durations;
	std::vector<CurrentPrices> updatesVec;

	std::unordered_set<int> minor_only;
	std::unordered_set<int> temp_only;
	std::unordered_set<int> perma_only;


	MasterNZOK();
	void loadData();

public:
	MasterNZOK(const MasterNZOK&) = delete;
	static MasterNZOK& instance();
	static void loadUpdates();

	int getDuration(int nzokCode);
	std::vector<ManipulationTemplate> getM_Templates(Date date, int specialty, bool adult, bool unfav);
	std::pair<patientPrice, nzokPrice> getPrices(int code, Date date, int specialty, bool adult, bool unfav);
	ManipulationTemplate getTemplateByCode(int code);

	bool isTempOnly(int code);
	bool isPermaOnly(int code);
	bool isMinorOnly(int code);
};

