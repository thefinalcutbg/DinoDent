#pragma once
#include <string>
#include "Model/Date.h"
#include "Model/Dental/MKB.h"

struct MedicalNotice
{
	std::string lrn;
	std::string nrn;
	
	static constexpr int reason = 1; //always sick
	static constexpr int location = 3; //always other

	Date issue_date{ Date::currentDate() };

	MKB mkb;

	std::string institution;

	Date from_date{ Date::currentDate() };
	Date to_date{ Date::currentDate() };

	std::string note;

};