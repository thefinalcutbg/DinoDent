#pragma once
#include <string>
#include <array>
#include <vector>
#include <algorithm>

#include "Model/Patient.h"
#include "Model/Date.h"
#include "Model/Time.h"
#include "ToothContainer.h"
#include "Model/Dental/ProcedureContainer.h"
#include "Model/Referrals/Referral.h"
#include "Model/Dental/NhifSheetData.h"

struct AmbList
{
	AmbList() {}

	long long rowid{ 0 };
	long long patient_rowid{ 0 };
	//Date date{ Date::currentDate() };
	Time time{ Time::currentTime() };

	int number{ 0 };
	std::string LPK;

	/* NHIF specific */
	NhifSheetData nhifData;

	ToothContainer teeth;
	ProcedureContainer procedures;

	std::vector<Referral> referrals;


	bool isNhifSheet() const
	{
		return (referrals.size() || procedures.hasNhifProcedure());
	}

	bool hasNumberInconsistency() const
	{
		return isNhifSheet() != number < 100000;
	}

	Date getDate() const //works only if procedures are sorted by date!
	{

		if (procedures.empty() && referrals.empty()) return Date::currentDate();

		if (isNhifSheet()) {

			Date date(1, 1, 2200);

			for (auto& p : procedures) if (p.isNhif()) date = std::min(date, p.date);

			for (auto& r : referrals) date = std::min(date, r.date);

			return date;

		}

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

	bool isNew() { return rowid == 0; }


	~AmbList() {  }

};