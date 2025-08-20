#pragma once
#include "NhifMaps.h"

class PackageCounter
{
	std::vector<ProcedurePackage> packages;
    std::vector<int> procedure_count;
public:
	PackageCounter(const std::vector<ProcedurePackage>& packages);
    void insertCode(int code);
    bool insertAndValidate(int code, bool adult);
	void setPregnantProperty();
};

