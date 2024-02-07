#pragma once

#include <unordered_map>

#include "Model/UserStructs.h"
#include "Model/Dental/NhifMaps.h"
#include "Model/Dental/NhifSheetData.h"
#include "Model/PlainTable.h"

struct Procedure;


class NhifSpec {

	NhifSpecificationType m_specification;
	NhifSpecialty m_specialty;

	std::unordered_map<PriceKey, int> m_procedures;

public:
	NhifSpec(NhifSpecificationType spec, NhifSpecialty specialty) :
		m_specification(spec),
		m_specialty(specialty)
	{}

	void addProcedure(const Procedure& p, bool adult);

};