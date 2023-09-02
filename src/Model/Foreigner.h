#pragma once
#include "Model/Date.h"
#include "Model/Country.h"

struct Foreigner 
{
	Country country;
	std::string city;
	std::string institution;
	std::string ehic;
	Date date_valid{ Date::currentDate() };
	
	bool isEHIC() const { return ehic.size(); }
};