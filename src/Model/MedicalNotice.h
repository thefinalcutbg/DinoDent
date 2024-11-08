#pragma once
#include <string>
#include "Model/Date.h"
#include "Model/ICD10.h"

struct MedicalNotice
{
	std::string lrn;
	std::string nrn;
	
	static constexpr int reason = 1; //always sick
	static constexpr int location = 3; //always other

	Date issue_date{ Date::currentDate() };

	ICD10 mkb;

	std::string institution;

	Date from_date{ Date::currentDate() };
	Date to_date{ Date::currentDate() };

	std::string note;

};