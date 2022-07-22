#pragma once

#include <vector>
#include "Model/Tooth/ToothContainer.h"
#include "Model/Procedure/Procedure.h"
#include "Model/PerioStatus.h"


enum class TimeFrameType{InitialAmb, Procedures, Perio};

struct TimeFrame
{
	TimeFrameType type;
	long long rowid{ 0 };
	std::string number;
	std::string LPK;
	Date date;
	ToothContainer teeth;
	std::vector<Procedure> procedures;
	PerioStatus perioData;

	std::string getFrameName() const;

};
