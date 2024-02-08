#pragma once
#include <string>

enum class NhifSpecificationType { PartialCoverage = 0, FullCoverage=1, Anesthesia=2 };
enum class NhifSpecialty;

struct NhifSheetData
{
	NhifSpecificationType specification{ NhifSpecificationType::PartialCoverage };
	std::string getSpecNumenclature(NhifSpecialty specialty) const;
	bool isUnfavourable{ false };
	void setSpecFromNhif(const std::string& spec);
};

