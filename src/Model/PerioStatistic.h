#pragma once
class PerioStatus;

class PerioStatistic
{

public:

	double HI;
	double BI;
	double BOP;

	double pdAverage;

	int pd3;
	int pd5;
	int pd7;
	int pdMore;

	int calAverage;
	double calSeverity;
	double calDistribution;

	int cal1to2;
	int cal3to4;
	int cal5andMore;

	PerioStatistic(const PerioStatus& status);



};


