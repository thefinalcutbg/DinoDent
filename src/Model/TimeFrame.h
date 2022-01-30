#pragma once

#include <vector>
#include "Model/Tooth/ToothContainer.h"
#include "Model/Procedure/Procedure.h"

struct TimeFrame
{
	std::string ambId;
	std::string LPK;
	Date date;
	ToothContainer teeth;
	std::vector<Procedure> procedures;
};
