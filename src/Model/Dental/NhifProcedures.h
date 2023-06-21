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
	std::vector<ProcedureCode> getNhifProcedures(Date date, NhifSpecialty specialty, bool adult, bool pregnancyAllowed, NhifSpecification specification);
	std::pair<patientPrice, nzokPrice> getPrices(int code, Date date, bool adult, NhifSpecialty doctorSpecialty, NhifSpecification specification);
	double getPatientPrice(int code, Date date, NhifSpecialty specialty, bool adult, NhifSpecification specification);
	double getNhifPrice(int code, Date date, NhifSpecialty specialty, bool adult, NhifSpecification specification);

	std::vector<ProcedurePackage> getPackages(Date ambDate);

	bool isTempOnly(int code);
	bool isPermaOnly(int code);
	bool isMinorOnly(int code);
};

