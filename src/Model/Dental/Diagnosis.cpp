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

std::string Diagnosis::getDiagnosisText() const
{
    auto& icdDescr = icd.name();

	if (icdDescr.empty()) {

        if (additional_descr.empty()) {

            return " ";
		}

		return additional_descr;
	}

	std::string result;
	result.reserve(50);
		
	result += icd.code();
	result += " - ";
	result += icd.name();

	if (additional_descr.size()) {
		result += " (";
		result += additional_descr;
		result += ")";
	}

    return result;
}

std::string Diagnosis::getDiagnosisTextNhif() const
{
	if (icd.isValid()) {
		return icd.code() + " - " + icd.name();
	}

    return "Без диагноза";
}
