#include "PerioStatistic.h"
#include "PerioStatus.h"
#include <utility>
#include <QDebug>
template<typename T>
double calculatePercent(const bool disabled[32], const T status[], const int statusSize, bool countExisting = true)
{
	int total = 0;
	double measure = 0;
	int step = statusSize / 32;

	for (int i = 0; i < statusSize; i++)
	{
		
		if (disabled[i/step]) {
			i += step;
			i--;       // because the loop will add one anyway
			continue;
		}

		if (static_cast<bool>(status[i]) == countExisting) measure += 1;

		total += 1;
	}
	
	return (measure / total) * 100;
}

double calculateAvg(const bool disabled[32], const int status[192])
{
	double sum = 0;
	int total = 0;

	for (int i = 0; i < 192; i++)
	{
		if (disabled[i / 6]) {
			i += 5; // loop ads one more position
			
			continue;
		}
		
		sum += status[i];
		total += 1;
	}

	return sum / total;
}

PerioStatistic::PerioStatistic(const PerioStatus& status) :
	HI{ calculatePercent(status.disabled, status.FMPS, 128, false) },
	BI{ calculatePercent(status.disabled, status.FMBS, 128) },
	BOP{ calculatePercent(status.disabled, status.bop, 192) },
	pdAverage{ calculateAvg(status.disabled, status.pd) },
	calAverage(calculateAvg(status.disabled, status.cal)),
	calDistribution{ calculatePercent(status.disabled, status.cal, 192) } 
{

	int calCount = 0;

	for (int i = 0; i < 192; i++)
	{
		auto& cal = status.cal[i];

		calMax = std::max(calMax, cal);

		if (!cal) continue;

		if (cal < 3)
			cal1to2++;
		else if (cal < 5)
			cal3to4++;
		else
			cal5andMore++;
	}

	for (int i = 0; i < 192; i++)
	{
		auto& pd = status.pd[i];
		if (pd <= 3)
			pd3++;
		else if (pd <= 5)
			pd5++;
		else if (pd <= 7)
			pd7++;
		else
			pdMore++;
	}

}
