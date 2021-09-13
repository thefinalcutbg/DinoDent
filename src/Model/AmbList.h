#pragma once
#include <string>
#include <array>
#include <vector>
#include "Patient.h"
#include "Date.h"
#include "Tooth/ToothContainer.h"
#include "Procedure/Procedure.h"


enum class Charge {standard, retired, freed};

struct AmbList
{
	AmbList() {}

	std::string id{"0"};
	Date date{ Date::currentDate() };
	int number{ 0 };
	std::string LPK;
	bool full_coverage{ false };
	bool pregnancy{ false };
	Charge charge{ Charge::standard };

	ToothContainer teeth;

	std::vector<Procedure> procedures;
	~AmbList() {  }
	bool isNew(){ return id == "0"; }
};