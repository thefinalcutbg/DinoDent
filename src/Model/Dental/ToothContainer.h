#pragma once
#include <vector>
#include "Tooth.h"
#include "Dental.h"

class ToothContainer
{

	std::vector<Tooth> teeth;

	static bool canResultInNonRetainedConstruction(int status);
	static bool needsBridgeFormatting(int status);
	void formatBridges(const std::vector<int>& selectedIndexes);
	void removeNonRetainedConstructions();


public:
	ToothContainer();

	auto begin() { return teeth.begin(); }
	auto end() { return teeth.end(); }
	auto begin() const { return teeth.begin(); }
	auto end() const { return teeth.end(); };

	size_t size() const { return teeth.size(); }

	std::vector<const Tooth*> getSelectedTeethPtr(std::vector<int> selectedIndexes) const;
	std::vector<const Tooth*> getSelectedDsnPtr(std::vector<int> selectedIndexes) const;

	void copyFromHis(const ToothContainer& other);
	void copyOnlyOnUnknown(const ToothContainer& other);

	//takes in consideration
	void setToothDetails(const Tooth& tooth);
	void removeBridgeOrSplint(const std::vector<int>& selectedIndexes);
	void removeBridgeOrSplint(int toothFrom, int toothTo);
	//void setStatus(const std::vector<int>& selectedTeethIdx, Dental::StatusCode code, bool state);
	void setStatus(const std::vector<int>& selectedIndexes, Dental::StatusType t, int code, bool state, bool dsn = false);
	void removeEveryStatus(const std::vector<int>& selectedTeethidx);

	Tooth& at(int index, bool supernumeral = false) {
		return supernumeral ? (*this)[index].getSupernumeral() : (*this)[index];
	}

	const Tooth& at(int index, bool supernumeral = false) const { 
		return supernumeral ? (*this)[index].getSupernumeral() : (*this)[index];
	}

	Tooth& at(const ToothIndex& idx) {
		return idx.supernumeral ? (*this)[idx.index].getSupernumeral() : (*this)[idx.index];
	}

	const Tooth& at(const ToothIndex& idx) const {
		return idx.supernumeral ? (*this)[idx.index].getSupernumeral() : (*this)[idx.index];
	}

	Tooth& operator [](int index);
	const Tooth& operator [](int tooth) const;

    bool noData() const;

	int getMissingTeethCount(bool countWisdom = false) const;
};

