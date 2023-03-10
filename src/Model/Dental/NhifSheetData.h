#pragma once
#include <string>

enum class NhifCharge { Standard, retired, freed };
enum class NhifSpecification { PartialCoverage = 0, FullCoverage=1, Anesthesia=2 };
enum class NhifSpecialty;

struct NhifSheetData
{
	NhifCharge charge{ NhifCharge::Standard };
	NhifSpecification specification{ NhifSpecification::PartialCoverage };
	std::string getSpecString(NhifSpecialty specialty) const;
	bool isUnfavourable{ false };
};

