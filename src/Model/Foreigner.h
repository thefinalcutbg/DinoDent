#pragma once
#include "Model/Date.h"
#include "Model/Country.h"

struct Foreigner 
{
	Country country;
	std::string institution;
	std::string ehic;
	Date date_valid;
	
	bool isEHIC() const { return ehic.size(); }
};