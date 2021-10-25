#include "PerioStatistic.h"
#include "PerioStatus.h"

double calculatePercent(const bool disabled[32], const bool status[], const int statusSize, bool measureExisting = true)
{
	double total = 0;
	double measure = 0;
	int step = statusSize / 32;

	for (int i = 0; i < statusSize; i++)
	{
		if (disabled[i/step]) {
			i += step;
			continue;
		}

		if (status[i] == measureExisting) measure += 1;

		total += 1;
	}

	return (measure / total) * 100;
}

double calculateAvg(const bool disabled[32], const int status[192])
{
	double sum = 0;

	for (int i = 0; i < 192; i++)
	{
		if (disabled[i / 6]) {
			i += 6;
			continue;
		}
		sum += status[i];
	}

	return sum / 192;
}

PerioStatistic::PerioStatistic(const PerioStatus& status) :
	HI{ calculatePercent(status.disabled, status.FMPS, 128, false) },
	BI{ calculatePercent(status.disabled, status.FMBS, 128) },
	BOP{ calculatePercent(status.disabled, status.bop, 192) },
	pdAverage{ calculateAvg(status.disabled, status.pd) },
	calAverage(calculateAvg(status.disabled, status.cal))
{


}
