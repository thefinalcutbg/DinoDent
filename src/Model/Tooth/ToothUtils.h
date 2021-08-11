#pragma once

#include "Enums.h"
#include <unordered_map>
#include <array>
#include <string>

class Tooth;

class ToothUtils
{
	static constexpr std::array<int, 32> numbers
	{ 
	  18, 17, 16, 15, 14, 13, 12, 11,     21, 22, 23, 24, 25, 26, 27, 28,
	  38, 37, 36, 35, 34, 33, 32, 31,     41, 42, 43, 44, 45, 46, 47, 48 
	};

	static constexpr std::array<int, 32> types
	{
		0,0,0,1,1,2,2,2,    2,2,2,1,1,0,0,0,
		0,0,0,1,1,2,2,2,    2,2,2,1,1,0,0,0
	};

	std::unordered_map<int, Quadrant> quadrant;

	static std::array<std::string, 6> surfaceNames;

public:
	ToothUtils();
	static ToothType getToothType(int index);
	static int getToothNumber(int index, bool temporary);
	static std::string getNomenclature(int index, bool temporary);
	static std::string getNomenclature(const Tooth& t);
	Quadrant getQuadrant(int index);
	std::array<std::string, 6> getSurfaceNames(int index);
};