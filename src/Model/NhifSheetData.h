#pragma once
#include <string>

enum class NhifCharge { standard, retired, freed };
enum class NhifSpecification { PartialCoverage, FullCoverage, Anesthesia };
enum class NhifSpecialty;

struct NhifSheetData
{
	bool pregnancy{ false };
	NhifCharge charge{ NhifCharge::standard };
	NhifSpecification specification{ NhifSpecification::PartialCoverage };
	std::string getSpecString(NhifSpecialty specialty) const;
};

