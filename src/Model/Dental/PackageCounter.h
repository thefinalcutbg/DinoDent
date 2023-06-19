#pragma once
#include "NhifMaps.h"

class PackageCounter
{
	const std::vector<ProcedurePackage> packages;
	std::vector<int> procedure_count;
public:
	PackageCounter(const std::vector<ProcedurePackage>& packages);
	void insertCode(int code);
	bool validate(bool adult);
};

