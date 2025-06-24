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
#include "Model/MedicalNotice.h"
#include "Model/Dental/NhifSheetData.h"
#include "Model/FreeFunctions.h"

struct AmbList
{
	AmbList() {}

	long long rowid{ 0 };
	long long patient_rowid{ 0 };
	
	std::string date{ FreeFn::getTimeStampLocal() };
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
	std::vector<MedicalNotice> medical_notices;

	bool isSigned{ false };

	bool isNhifSheet() const
	{
		return (referrals.size() || procedures.hasNhifProcedure());
	}

	Date getDate() const
	{
		return Date(date);
	}

	Date getAmbSheetDateMin() const
	{ 
		auto date = Date(this->date);
		return Date(1, date.month, date.year);

	}

	Date getLastDateVisit() const {

		Date result = getDate();

		for (auto& p : procedures) {
			result = std::max(result, p.date);
		}
		
		for (auto& r : referrals) {
			result = std::max(result, r.date);
		}
		
		return result;
	}

	bool isNew() { return rowid == 0; }

	std::string getNumber() const {

		if (nrn.size()) return nrn;

		return FreeFn::leadZeroes(number, 6);
	}

	~AmbList() {  }

};