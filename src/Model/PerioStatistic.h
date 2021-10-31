#pragma once
class PerioStatus;
#include <array>

class PerioStatistic
{

public:

	double HI{ 0 }, BI{ 0 }, BOP{ 0 }, pdAverage{ 0 }, calAverage{ 0 }, calDistribution{ 0 };

	int calMax{ 0 };

	std::array<int, 4> pdHistogramCount;
	std::array<int, 3> calHistogramCount;

	std::array<double, 4> pdHistogramPercentage;
	std::array<double, 3> calHistogramPercentage;

	PerioStatistic(const PerioStatus& status);



};


