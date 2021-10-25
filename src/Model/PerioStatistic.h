#pragma once
class PerioStatus;

class PerioStatistic
{

public:

	double HI, BI, BOP, pdAverage, calAverage, calDistribution;

	int calMax{ 0 };

	int pd3{ 0 }, pd5{ 0 }, pd7{ 0 }, pdMore{ 0 };
	int cal1to2{ 0 }, cal3to4{ 0 }, cal5andMore{ 0 };

	PerioStatistic(const PerioStatus& status);



};


