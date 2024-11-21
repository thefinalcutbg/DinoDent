#pragma once
#include <vector>
#include <string>
#include "Model/Date.h"
#include "Model/Dental/ProcedureCode.h"

struct DentalStatistic
{
	int year{ 0 };

	enum class AgeFilter{ None, Minor, Adult };
	enum class ToothFilter { All, Permanent, Temporary };
	enum class FinancingFilter { All, Nhif, Cash };

	AgeFilter age{ AgeFilter::None };
	ToothFilter tooth{ ToothFilter::All };
	FinancingFilter financing{ FinancingFilter::All };

	bool nonTooth_specific{ false };

	std::vector<std::string> diagnosisFilter;
	std::vector<std::string> procedureCodeFilter;

	int count{ 0 };
};