#pragma once
#include <string>
#include "Model/Date.h"
#include "Model/Dental/MKB.h"

struct MedicalNotice
{
	enum Location { AtHome = 1, Facility, Other };

	long long roiwd{ 0 };
	std::string lnr;
	std::string nrn;
	
	static constexpr int reason = 1; //always sick

	Date issue_date{ Date::currentDate() };
	
	Location location{ Location::Other };

	Date from_date{ Date::currentDate() };
	Date to_date{ Date::currentDate() };

	MKB mkb;

	std::string institution;
	std::string note;

};