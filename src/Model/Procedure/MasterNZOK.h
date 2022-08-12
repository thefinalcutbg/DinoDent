#pragma once
#include <vector>
#include <unordered_set>
#include "ProcedureTemplate.h"
#include "NZOKmaps.h"
#include "Model/NhifSheetData.h"

class MasterNZOK
{
	static MasterNZOK _instance;


	

	MasterNZOK();
	void loadData();

public:
	MasterNZOK(const MasterNZOK&) = delete;
	static MasterNZOK& instance();

	void loadUpdates();

	int getDuration(int nzokCode);
	int getYearLimit(int nzokCode);
	std::vector<ProcedureTemplate> getM_Templates(Date date, NhifSpecialty specialty, bool adult, NhifSpecification specification);
	std::pair<patientPrice, nzokPrice> getPrices(int code, Date date, bool adult, NhifSpecialty doctorSpecialty, NhifSpecification specification);
	double getPatientPrice(int code, Date date, NhifSpecialty specialty, bool adult, NhifSpecification specification);
	double getNZOKPrice(int code, Date date, NhifSpecialty specialty, bool adult, NhifSpecification specification);

	std::vector<ProcedurePackage> getPackages(Date ambDate);

	ProcedureTemplate getTemplateByCode(int code);

	bool isTempOnly(int code);
	bool isPermaOnly(int code);
	bool isMinorOnly(int code);
};

