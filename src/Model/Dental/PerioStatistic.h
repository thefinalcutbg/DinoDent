#pragma once

class ToothContainer;
struct PerioStatus;

#include <array>

enum class Stage { Healthy, First, Second, Third, Fourth };

enum class RiskGrade { Low, Moderate, High};

class PerioStatistic
{

public:

	double	HI, 
			BI, 
			BOP, 
			pdAverage, 
			calAverage, 
			calDistribution,
			boneIdx;
					

	int		calMax,
			pdMax,
			furcMax,
			missingTeeth;

	std::array<int, 4> pdHistogramCount;
	std::array<int, 3> calHistogramCount;

	std::array<double, 4> pdHistogramPercentage;
	std::array<double, 3> calHistogramPercentage;

	bool localized{ false };

	Stage stage{ Stage::Healthy };

	std::array<int, 6> riskHexagon;

	RiskGrade risk;

    PerioStatistic(const PerioStatus& status, int age);

};


