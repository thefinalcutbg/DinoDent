#pragma once

#include "Enums.h"
#include <unordered_map>
#include <array>
#include <string>
#include <utility>
#include "ToothIndex.h"

class Tooth;

namespace ToothUtils
{
	ToothType getToothType(int index);
	int getToothNumber(int index, bool temporary);
	std::string getNomenclature(int index, bool temporary = false);
	std::string getNomenclature(const Tooth& t);
	std::string getNhifNumber(int index, bool temporary = false, bool hyperdontic = false);
	Quadrant getQuadrant(int index);
	std::array<std::string, 6> getSurfaceNames(int index);
	ToothIndex getToothFromNhifNum(const std::string& toothNum);
	ToothIndex getToothFromHisNum(const std::string& toothNum, bool hyperdontic);
	std::string getName(int idx, bool temp);
	const std::array<int, 32>& FDINumbers();
};