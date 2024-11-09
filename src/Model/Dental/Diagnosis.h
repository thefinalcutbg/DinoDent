#pragma once
#include "Model/ICD10.h"

struct Diagnosis
{
	ICD10 icd;

	std::string additional_descr;

	Diagnosis() {};

	Diagnosis(int legacyHisIdx, bool refactorGuard);

	const std::string& getDiagnosisText() const;
};