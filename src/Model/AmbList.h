#pragma once
#include <string>
#include <array>
#include "Patient.h"
#include "Date.h"
#include "Tooth/Tooth.h"

struct AmbList
{
	AmbList() : date(1, 1, 1900), number(0) 
	{
		for (int i = 0; i < teeth.size(); i++) {
			teeth[i].index = i;
		}
	}

	std::string id;
	Date date;
	int number;
	std::string LPK;
	std::string test;

	std::array <Tooth, 32> teeth;
	~AmbList() {  }
	bool isNew(){ return !number; }
};