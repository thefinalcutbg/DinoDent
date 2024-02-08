#pragma once

#include <map>

#include "Model/UserStructs.h"
#include "Model/Dental/NhifMaps.h"
#include "Model/Dental/NhifSheetData.h"
#include "Model/PlainTable.h"
#include "Model/Date.h"

struct Procedure;

class NhifSpecReport {

	std::map<int, int> procedures_minor;
	std::map<int, int> procedures_adult;

public:

	const std::string doctor_name;
	const NhifSpecialty m_specialty;
	const Date dateFrom;
	const Date dateTo;
	const NhifSpecificationType m_specificationType;

	NhifSpecReport(const Doctor& d, Date reportDate, NhifSpecificationType specType);

	void addProcedure(const Procedure& p, bool adult, NhifSpecificationType spec);

	std::string getSpecString() const;

	PlainTable getSpecificationReport() const;


};