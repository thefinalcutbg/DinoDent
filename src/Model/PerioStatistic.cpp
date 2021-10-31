#include "PerioStatistic.h"
#include "PerioStatus.h"
#include <utility>
#include <functional>

inline double getPercent(int sum, int total) { return total ? (static_cast<double>(sum) / total) * 100 : 0; }

constexpr int teethCount = 32;

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
		m_array(array), size(size), disabled(disabled), step(size / 32), index{ 0 }
	{
		for (int i = 0; i < teethCount; i++)
		{
			if (!disabled[i]) break;

			index += step;
		}
	}

	void increment()
	{
		index++;

		for (int i = index / step; i < teethCount; i++)
		{
			if (!disabled[i]) break;

			index += step;
		}
	}

	bool valid() { return index < size; }

	const T& value() { return m_array[index]; }

};

template<typename T>
double calculatePercent(PerioIterator<T>& data, bool countExisting = true)
{
	int total = 0;
	int sum = 0;

	for (; data.valid(); data.increment())
	{
		if (static_cast<bool>(data.value()) == countExisting)
				sum++;

			total++;
	}
	
	return getPercent(sum, total);
}

double calculateAvg(PerioIterator<int>& data)
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


typedef std::function<bool(int)> ConditionLambda;

template <size_t histoCount>
std::array<int, histoCount> getHistogram(
	PerioIterator<int>& data,
	std::array<ConditionLambda, histoCount>& lambdas
	)
{
	std::array<int, histoCount> result{ 0 };

	for (; data.valid(); data.increment())
	{
		for (int y = 0; y < lambdas.size(); y++)
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

	for (int i = 0; i < size; i++)
		result[i] = getPercent(histogram[i], total);

	return result;

}

PerioStatistic::PerioStatistic(const PerioStatus& status) :

	HI{ calculatePercent(PerioIterator{status.FMPS, 128 ,status.disabled}, false) },
	BI{ calculatePercent(PerioIterator{status.FMBS, 128, status.disabled}) },
	BOP{ calculatePercent(PerioIterator{status.bop, 192, status.disabled}) },
	calAverage(calculateAvg(PerioIterator{ status.cal, 192,status.disabled })),
	calDistribution{ calculatePercent(PerioIterator{status.cal, 192, status.disabled}) },
	pdAverage{ calculateAvg(PerioIterator{ status.pd, 192, status.disabled }) },

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
	calHistogramPercentage{histogramToPercentage(calHistogramCount)}

{}