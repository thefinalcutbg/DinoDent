#pragma once
#include <vector>
#include "Tooth.h"

static constexpr int teethCount = 32;

class ToothContainer
{

	std::vector<Tooth> teeth;

public:
	ToothContainer();

	void formatBridges(const std::vector<int>& selectedIndexes);
	void removeBridge(int tooth_idx);
	void removeBridgeOrSplint(int tooth_begin, int tooth_end);
	void setToothDetails(const Tooth& tooth);

	auto begin() { return teeth.begin(); }
	auto end() { return teeth.end(); }
	auto begin() const { return teeth.begin(); }
	auto end() const { return teeth.end(); };

	constexpr size_t size() const { return teethCount; }

	std::vector<Tooth*> getSelectedTeethPtr(std::vector<int> selectedIndexes);

	Tooth& at(int index) { return (*this)[index]; }
	const Tooth& at(int index) const { return(*this)[index]; }


	const Tooth& operator [](int tooth) const;
	Tooth& operator [](int index);

	int getMissingTeethCount(bool countWisdom = false) const;


};

