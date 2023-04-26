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
	
	std::string date{ FreeFn::getTimeStamp() };
	std::string lrn;
	std::string nrn;
	std::string basedOn;

	bool his_updated{ false };

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

	Date getDate() const //works only if procedures are sorted by date!
	{
		return Date(date);
	}

	Date getAmbSheetDateMin() const //returns the first day of the ambSheet month
	{ 
		auto date = Date(this->date);
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

	std::string getNumber() const {
		if (nrn.size()) return nrn;

		return FreeFn::leadZeroes(number, 6);
	}

	~AmbList() {  }

};