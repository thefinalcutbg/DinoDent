#pragma once

#include <unordered_map>

#include "Model/UserStructs.h"
#include "Model/Dental/NhifMaps.h"
#include "Model/Dental/NhifSheetData.h"
#include "Model/PlainTable.h"

struct Procedure;


class NhifSpecReport {

	std::unordered_map<PriceKey, int> procedure_map;

public:

	NhifSpecialty m_specialty;
	Date m_reportDate;

	NhifSpecReport(NhifSpecialty specialty, Date reportDate) :
		m_specialty(specialty),
		m_reportDate(reportDate)
	{
		procedure_map.reserve(100);
	}

	void addProcedure(const Procedure& p, bool adult, NhifSpecificationType spec);

	PlainTable getSpecificationReport() const;
	double getTotalPrice() const;

};