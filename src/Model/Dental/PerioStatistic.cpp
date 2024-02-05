#include "PerioStatistic.h"
#include "PerioStatus.h"
#include "Model/Dental/ToothContainer.h"
#include <utility>
#include <functional>

inline double getPercent(int sum, int total) { return total ? (static_cast<double>(sum) / total) * 100 : 0; }

template<typename T>
class PerioIterator
{
	const T* m_array;
	int size;
	int step;
	const bool* disabled;
	int index;
public:

	PerioIterator(const T* array, int size, const bool* disabled)
		:
        m_array(array), size(size), step(size / Dental::teethCount), disabled(disabled), index{ 0 }
	{
		for (int i = 0; i < Dental::teethCount; i++)
		{
			if (!disabled[i]) break;

			index += step;
		}
	}

	void increment()
	{
		index++;

		for (int i = index / step; i < Dental::teethCount; i++)
		{
			if (!disabled[i]) break;

			index += step;
		}
	}

	bool valueIsInterdental() { return index % 6 != 1 && index % 6 != 4; }

	bool valid() const { return index < size; }

	const T& value() const { return m_array[index]; }

};

template<typename T>
double calculatePercent(PerioIterator<T> data, bool countExisting = true)
{
	int total = 0;
	int sum = 0;

	for (; data.valid(); data.increment())
	{
		if (static_cast<bool>(data.value()) == countExisting) sum++;

		total++;
	}
	
	return getPercent(sum, total);
}

double calculateAvg(PerioIterator<int> data)
{
	double sum = 0;
	int total = 0;

	for (;data.valid(); data.increment())
	{
		sum += data.value();
		total += 1;
	}

	return sum ? sum / total : 0;
}

int calculateMax(PerioIterator<int> data)
{
	int max{ 0 };

	for (; data.valid(); data.increment())
		max = std::max(max, data.value());

	return max;
}


typedef std::function<bool(int)> ConditionLambda;

template <size_t histoCount>
std::array<int, histoCount> getHistogram(
	PerioIterator<int> data,
	const std::array<ConditionLambda, histoCount>& lambdas
	)
{
	std::array<int, histoCount> result{ 0 };

	for (; data.valid(); data.increment())
	{
        for (size_t y = 0; y < lambdas.size(); y++)
		{
			if (lambdas[y](data.value())) {
				result[y]++;
				break;
			}
		}

	}

	return result;

}

template<size_t size>
std::array<double, size> histogramToPercentage(const std::array<int, size>& histogram)
{
	int total{ 0 };

	for (const auto& value : histogram)
		total += value;

	std::array<double, size> result{ 0 };

    for (size_t i = 0; i < size; i++)
		result[i] = getPercent(histogram[i], total);

	return result;

}

int getMissingTeethCount(const bool* disabled)
{
	int missingTeeth{ 0 };

	for (int i = 0; i < 32; i++)
	{
		if (i == 0 || i == 15 || i == 16 || i == 31) continue; //skipping the wisdom teeth

		if(disabled[i])
			missingTeeth++;
	}

	return missingTeeth;
}

//stage calculator
Stage getStage(const PerioStatistic& stat, bool restorationNeeded)
{
	auto& calMax = stat.calMax;
	auto& pdMax = stat.pdMax;
	auto& furcMax = stat.furcMax;

	auto severity(Stage::Healthy);

	if (!calMax) return severity;
	else if (calMax < 3)
		severity = Stage::First;
	else if (calMax < 5)
		severity = Stage::Second;
	else
		severity = Stage::Third;

	auto complexity(Stage::First);

	if (restorationNeeded) 
		complexity = Stage::Fourth;
	else if (pdMax > 5 || furcMax > 1)
		complexity = Stage::Third;
	else if (pdMax == 5)
		complexity = Stage::Second;


	
	return std::max(severity, complexity);

}


//risk hexagon value calculator
template<typename T, size_t size>
int getValue(const std::array<T, size>& groupings, T parameter)
{
    for (size_t i = 0; i < size; i++)
		if (parameter <= groupings[i])
			return i;

	return size-1;
}

//risk hexagon calculator
RiskGrade getRisk(const std::array<int, 6>& riskHexagon)
{
	RiskGrade risk(RiskGrade::High);

	int lowCount{ 0 }, moderateCount{ 0 }, highCount{ 0 };

	for (int i = 0; i < 6; i++)
	{
		if (riskHexagon[i] < 2)
			lowCount++;
		else if (riskHexagon[i] < 4)
			moderateCount++;
		else
			highCount++;
	}

	if (lowCount >= 5 && moderateCount <= 1 && highCount == 0)
		risk = RiskGrade::Low;
	else if (highCount < 2)
		risk = RiskGrade::Moderate;
	else if(highCount >= 2)
		risk = RiskGrade::High;

	return risk;
}

PerioStatistic::PerioStatistic(const PerioStatus& status, int age) :

	HI{ calculatePercent(PerioIterator{status.FMPS, 128, status.disabled}, false) },
	BI{ calculatePercent(PerioIterator{status.FMBS, 128, status.disabled}) },
	BOP{ calculatePercent(PerioIterator{status.bop, 192, status.disabled}) },
    pdAverage{ calculateAvg(PerioIterator{ status.pd, 192, status.disabled }) },
    calAverage(calculateAvg(PerioIterator{status.cal, 192,status.disabled })),
    calDistribution{ calculatePercent(PerioIterator{status.cal, 192, status.disabled}) },

    boneIdx{static_cast<double>(status.boneLoss)/age},

    calMax{calculateMax(PerioIterator{ status.cal, 192,status.disabled})},

    pdMax{calculateMax(PerioIterator{ status.pd, 192,status.disabled}) },
    furcMax{calculateMax(PerioIterator{status.furcation, 96, status.disabled})},

    missingTeeth(getMissingTeethCount(status.disabled)),
    pdHistogramCount{
        getHistogram(
            PerioIterator{status.pd, 192, status.disabled},

            std::array<ConditionLambda, 4> {
                [](int value) { return value <= 3; },
                [](int value) { return value <= 5; },
                [](int value) { return value <= 7; },
                [](int value) { return value > 7; }
            }
            )
    },
    calHistogramCount{
        getHistogram(
            PerioIterator{status.cal, 192, status.disabled},

            std::array<ConditionLambda, 3> {
                [](int value) { return value > 0 && value <= 2; },
                [](int value) { return value > 2 && value <= 4; },
                [](int value) { return value > 4; }
            }
            )
    },

    pdHistogramPercentage{ histogramToPercentage(pdHistogramCount) },

    calHistogramPercentage{histogramToPercentage(calHistogramCount)},
    localized{calDistribution < 30},

    stage{ getStage(*this, status.completeRestorationNeeded)},

	riskHexagon{
		
		getValue(
		std::array<double, 6>{4, 9, 16, 25, 36, 50},
		BOP
		),

		getValue(
			std::array<int, 6>{ 2, 4, 6, 8, 10, 12},
		    pdHistogramCount[2] + pdHistogramCount[3]
		),

		getValue(
			std::array<int, 6>{ 2, 4, 6, 8, 10, 12},
			missingTeeth
		),

		getValue(
			std::array<double, 6>{ 0.25, 0.5, 0.75, 1, 1.25, 1.5 },
			boneIdx
		),

		status.systemic ? 5 : 0,

		status.smoker
	},

	risk{getRisk(riskHexagon)}
{
		

}
