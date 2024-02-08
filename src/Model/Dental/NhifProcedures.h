#pragma once
#include <vector>
#include <unordered_set>
#include "NhifMaps.h"
#include "Model/Dental/NhifSheetData.h"
#include "Model/Dental/Procedure.h"

namespace NhifProcedures
{

	void initialize();
	int getDuration(int nzokCode);
	int getYearLimit(int nzokCode);
	std::vector<ProcedureCode> getNhifProcedures(Date date, NhifSpecialty specialty, bool adult, bool pregnancyAllowed, NhifSpecificationType specification);
	std::pair<patientPrice, nzokPrice> getPrices(int code, Date date, bool adult, NhifSpecialty doctorSpecialty, NhifSpecificationType specification);
	double getPatientPrice(int code, Date date, NhifSpecialty specialty, bool adult, NhifSpecificationType specification);
	double getNhifPrice(int code, Date date, NhifSpecialty specialty, bool adult, NhifSpecificationType specification);
	PriceValue getPriceValue(const PriceKey& key, const Date& date);
	std::vector<ProcedurePackage> getPackages(Date ambDate);

	bool isTempOnly(int code);
	bool isPermaOnly(int code);
	bool isMinorOnly(int code);
};

