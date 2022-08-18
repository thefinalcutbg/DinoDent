#pragma once
#include <string>
#include <array>
#include <vector>
#include <algorithm>

#include "Patient.h"
#include "Date.h"
#include "Time.h"
#include "Tooth/ToothContainer.h"
#include "Procedure/ProcedureContainer.h"

#include "Model/NhifSheetData.h"

struct AmbList
{
	AmbList() {}

	long long rowid{ 0 };
	long long patient_rowid{ 0 };
	//Date date{ Date::currentDate() };
	Time time{Time::currentTime()};

	int number{ 0 };
	std::string LPK;

	/* NHIF specific */
	NhifSheetData nhifData;

	ToothContainer teeth;
	ProcedureContainer procedures;

	bool hasNZOKProcedure() const
	{
		return procedures.hasNzokProcedure();
	}

	bool hasNumberInconsistency() const
	{
		return hasNZOKProcedure() != number < 100000;
	}

	Date getDate() const //works only if procedures are sorted by date!
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
		auto date = getDate();

		return Date(1, date.month, date.year);

	}

	Date newProcedureDate() const
	{
		auto sheetDate = getDate();

		if (sheetDate.month == Date::currentMonth()
			&& sheetDate.year == Date::currentYear()) {

			return Date::currentDate();
		}

		return sheetDate;
			
	}

	~AmbList() {  }
	bool isNew(){ return rowid == 0; }
};