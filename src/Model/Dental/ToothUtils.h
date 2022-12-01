#pragma once

#include "Enums.h"
#include <unordered_map>
#include <array>
#include <string>
#include <utility>

class Tooth;

namespace ToothUtils
{

	ToothType getToothType(int index);
	int getToothNumber(int index, bool temporary);
	std::string getNomenclature(int index, bool temporary = false);
	std::string getNomenclature(const Tooth& t);
	Quadrant getQuadrant(int index);
	std::array<std::string, 6> getSurfaceNames(int index);
	std::pair<int, bool> getArrayIdxAndTemp(int index);
	std::string getName(int idx, bool temp);
	const std::array<int, 32>& getToothNumbers();
};