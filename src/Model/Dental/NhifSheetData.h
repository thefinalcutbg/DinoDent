#pragma once
#include <string>

enum class NhifSpecification { PartialCoverage = 0, FullCoverage=1, Anesthesia=2 };
enum class NhifSpecialty;

struct NhifSheetData
{
	NhifSpecification specification{ NhifSpecification::PartialCoverage };
	std::string getSpecString(NhifSpecialty specialty) const;
	bool isUnfavourable{ false };
	void setSpecFromNhif(const std::string& spec);
};

