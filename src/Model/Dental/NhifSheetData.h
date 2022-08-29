#pragma once
#include <string>

enum class NhifCharge { Standard, retired, freed };
enum class NhifSpecification { PartialCoverage, FullCoverage, Anesthesia };
enum class NhifSpecialty;

struct NhifSheetData
{
	NhifCharge charge{ NhifCharge::Standard };
	NhifSpecification specification{ NhifSpecification::PartialCoverage };
	std::string getSpecString(NhifSpecialty specialty) const;
};

