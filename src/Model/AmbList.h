#pragma once
#include <string>
#include <array>
#include <vector>
#include "Patient.h"
#include "Date.h"
#include "Tooth/Tooth.h"
#include "Procedure/Procedure.h"


constexpr int defaultSurfaces[32] = { 0,0,0,0,0,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,0,0,0,0,0 };

enum class Charge {default, retired, freed};

struct AmbList
{
	AmbList() : date(Date::currentDate()), number(0), full_coverage(false), charge{Charge::default}
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
	bool full_coverage;
	Charge charge;

	std::vector<Procedure> manipulations;
	std::array <Tooth, 32> teeth;
	~AmbList() {  }
	bool isNew(){ return !number; }
};