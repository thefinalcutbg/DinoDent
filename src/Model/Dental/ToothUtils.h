#pragma once

#include "Enums.h"
#include <unordered_map>
#include <array>
#include <string>
#include <utility>

class Tooth;

namespace ToothUtils
{
	struct ToothProcedureCode {
		int tooth;
		bool temporary;
		bool hyperdontic;
	};

	ToothType getToothType(int index);
	int getToothNumber(int index, bool temporary);
	std::string getNomenclature(int index, bool temporary = false);
	std::string getNomenclature(const Tooth& t);
	std::string getNhifNumber(int index, bool temporary = false, bool hyperdontic = false);
	Quadrant getQuadrant(int index);
	std::array<std::string, 6> getSurfaceNames(int index);
	ToothProcedureCode getToothFromNhifNum(const std::string& toothNum);
	std::string getName(int idx, bool temp);
	const std::array<int, 32>& FDINumbers();
};