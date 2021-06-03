#pragma once
#include <string>
#include <array>
#include <vector>
#include "Patient.h"
#include "Date.h"
#include "Tooth/Tooth.h"
#include "Manipulation/Manipulation.h"


constexpr int defaultSurfaces[32] = { 0,0,0,0,0,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,0,0,0,0,0 };

struct AmbList
{
	AmbList() : date(1, 1, 1900), number(0), unfavourable(false)
	{
		for (int i = 0; i < teeth.size(); i++) {
			teeth[i].index = i;
			teeth[i].caries.setDefaultSurface(defaultSurfaces[i]);
			teeth[i].obturation.setDefaultSurface(defaultSurfaces[i]);
		}
	}

	std::string id;
	Date date;
	int number;
	std::string LPK;
	bool unfavourable;

	std::vector<Manipulation> manipulations;
	std::array <Tooth, 32> teeth;
	~AmbList() {  }
	bool isNew(){ return !number; }
};