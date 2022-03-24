#pragma once
#include <string>
#include <array>
#include <vector>
#include "Patient.h"
#include "Date.h"
#include "Tooth/ToothContainer.h"
#include "Procedure/Procedure.h"
#include <algorithm>

enum class Charge {standard, retired, freed};

typedef std::vector<Procedure> Procedures;

struct AmbList
{
	AmbList() {}

	std::string id{"0"};
	//Date date{ Date::currentDate() };
	int number{ 0 };
	std::string LPK;
	bool full_coverage{ false };
	bool pregnancy{ false };
	Charge charge{ Charge::standard };

	ToothContainer teeth;

	bool hasNZOKProcedure() const
	{
		for (auto p : procedures)
			if (p.nzok)
				return true;

		return false;
	}

	bool hasNumberInconsistency() const
	{
		return hasNZOKProcedure() != number < 100000;
	}

	Date getAmbListDate() const //works only if procedures are sorted by date!
	{
		if (procedures.empty())
			return Date::currentDate();

		for (auto& p : procedures)
			if (p.nzok)
				return p.date;
		
		return procedures[0].date;
	}

	Date getAmbSheetDateMin() const //returns the first day of the ambSheet month
	{
		auto date = getAmbListDate();

		return Date(1, date.month, date.year);

	}

	Date newProcedureDate() const
	{
		auto sheetDate = getAmbListDate();

		if (sheetDate.month == Date::currentMonth()
			&& sheetDate.year == Date::currentYear()) {

			return Date::currentDate();
		}

		return sheetDate;
			
	}

	Procedures procedures;
	~AmbList() {  }
	bool isNew(){ return id == "0"; }
};