#pragma once
#include <vector>
#include <string>
#include "Model/Date.h"

struct DentalStatistic
{
	std::string name;

	enum AgeFilter { None, Minor, Adult };
	enum ToothFilter { All, Permanent, Temporary };

	int procedureType{ 0 };
	AgeFilter age{ None };
	ToothFilter tooth{ All };
	bool nhifOnly{ false };

	std::vector<std::string> diagnosisFilter;
	std::vector<std::string> procedureNameFilter;
};