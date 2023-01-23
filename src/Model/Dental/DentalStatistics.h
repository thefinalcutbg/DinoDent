#pragma once
#include <vector>
#include <string>
#include "Model/Date.h"

struct DentalStatistic
{

	enum AgeFilter { None, Minor, Adult };
	enum ToothFilter { All, Permanent, Temporary };

	std::string name;

	int procedureType{ 0 };

	AgeFilter age{ None };
	ToothFilter tooth{ All };
	bool nhifOnly{ false };

	std::vector<std::string> diagnosisFilter;
	std::vector<std::string> procedureNameFilter;

	int count{ 0 };
};