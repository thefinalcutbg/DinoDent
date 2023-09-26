#pragma once
#include <string>

#include "Model/Dental/Enums.h"

class ToothContainer;

struct SupernumeralPrint
{
	SupernumeralPrint(Quadrant q, const ToothContainer& teeth);

	//contains only position and not quadrant
	int position{ 0 };
	std::string printStatus;

	bool isValid() const {
		return position != 0;
	}
};
