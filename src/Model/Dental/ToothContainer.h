#pragma once
#include <vector>
#include "Tooth.h"

static constexpr int teethCount = 32;

class ToothContainer
{

	std::vector<Tooth> teeth;

	static bool canResultInNonRetainedConstruction(int status);
	static bool needsBridgeFormatting(int status);
	void formatBridges(const std::vector<int>& selectedIndexes);
	void removeNonRetainedConstruction();


public:
	ToothContainer();

	auto begin() { return teeth.begin(); }
	auto end() { return teeth.end(); }
	auto begin() const { return teeth.begin(); }
	auto end() const { return teeth.end(); };

	constexpr size_t size() const { return teethCount; }

	std::vector<const Tooth*> getSelectedTeethPtr(std::vector<int> selectedIndexes) const;
	std::vector<const Tooth*> getSelectedDsnPtr(std::vector<int> selectedIndexes) const;

	void copyFromOther(const ToothContainer& other);

	//takes in consideration
	void setToothDetails(const Tooth& tooth);
	void removeBridgeOrSplint(const std::vector<int>& selectedIndexes);
	void removeBridgeOrSplint(int tooth_begin, int tooth_end);
	void setStatus(const std::vector<int>& selectedTeethIdx, StatusCode::StatusCode code, bool state);
	void removeEveryStatus(const std::vector<int>& selectedTeethidx);

	Tooth& at(int index) { return (*this)[index]; }
	const Tooth& at(int index) const { return(*this)[index]; }

	Tooth& operator [](int index);
	const Tooth& operator [](int tooth) const;
	
	int getMissingTeethCount(bool countWisdom = false) const;


};

