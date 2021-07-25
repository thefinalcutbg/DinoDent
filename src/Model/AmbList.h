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
	AmbList()
	{
		for (int i = 0; i < teeth.size(); i++) {
			teeth[i].index = i;
			teeth[i].caries.setDefaultSurface(defaultSurfaces[i]);
			teeth[i].obturation.setDefaultSurface(defaultSurfaces[i]);
		}
	}

	std::string id{"0"};
	Date date{ Date::currentDate() };
	int number{ 0 };
	std::string LPK;
	bool full_coverage{ false };
	bool pregnancy{ false };
	Charge charge{ Charge::default };

	std::vector<Procedure> procedures;
	std::array <Tooth, 32> teeth;
	~AmbList() {  }
	bool isNew(){ return id == "0"; }
};