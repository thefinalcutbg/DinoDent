#pragma once

#include "Tooth.h"
#include "Enums.h"
#include <unordered_map>


class ToothUtils
{
	std::unordered_map<int, ToothType> toothTypeMap;
	std::unordered_map<int, int> permaMap;
	std::unordered_map<int, int> tempMap;
	std::unordered_map<int, Quadrant> quadrant;
	std::array<std::string, 6> surfaceNames;

public:
	ToothUtils();
	ToothType getToothType(int index);
	int getToothNumber(const int& index, const bool& temporary);
	Quadrant getQuadrant(int index);
	std::array<std::string, 6> getSurfaceNames(int index);
};