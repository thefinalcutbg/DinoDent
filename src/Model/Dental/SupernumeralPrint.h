#pragma once
#include <string>

#include "Model/Dental/Dental.h"

class ToothContainer;

struct SupernumeralPrint
{
	SupernumeralPrint(Dental::Quadrant q, const ToothContainer& teeth);

	//contains only position and not quadrant
	int position{ 0 };
	std::string printStatus;

	bool isValid() const {
		return position != 0;
	}
};
