#pragma once

#include <vector>
#include "Manipulation.h"
#include "NZOKmaps.h"

class MasterNZOK
{
	std::unordered_map<int, ManipulationTemplate> _manipulations;
	std::unordered_map<int, int> code_durations;
	std::vector<CurrentPrices> updatesVec;

public:
	MasterNZOK();

	int getDuration(int nzokCode);
	std::vector<ManipulationTemplate> getM_Templates(Date date, int specialty, bool adult, bool unfav);
	std::pair<patientPrice, nzokPrice> getPrices(int code, Date date, int specialty, bool adult, bool unfav);
};

