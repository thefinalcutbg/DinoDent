#include "Diagnosis.h"

Diagnosis::Diagnosis(int legacyHisIdx, bool refactorGuard)
{

	static const std::pair<std::string, std::string> legacyMap[11] =
	{
		{"", ""},
		{"K02", ""},
		{"K04.0", ""},
		{"K04.5", ""},
		{"S02.5", ""},
		{"K00.0", "Anodontia partialis"},
		{"K00.0", "Anodontia totalis"},
		{"K05", ""},
		{"K04.9", "Status post devitalisationem"},
		{"K04.9", "Devitalisatio pro causa prothetica" },
		{"K03.6", "" }
	};	

	if (legacyHisIdx > 10) {
		return;
	}

	icd = ICD10(legacyMap[legacyHisIdx].first);
	additional_descr = legacyMap[legacyHisIdx].second;
}

const std::string& Diagnosis::getDiagnosisText() const
{
	if (additional_descr.size()) return additional_descr;

	static const std::string dummy = "Без диагноза";

	auto& result = icd.name();

	if (result.empty()) {

		return dummy;
	}

	return result;
}
