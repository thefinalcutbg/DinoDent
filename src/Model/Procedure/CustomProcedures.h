#pragma once
#include <vector>
#include "ProcedureTemplate.h"

class CustomProcedures
{
	static CustomProcedures _instance;
	std::vector<ProcedureTemplate> _customProcedures;

	CustomProcedures();

public:
	CustomProcedures(const CustomProcedures&) = delete;
	static CustomProcedures& instance();

	void loadCustomProcedures();
	const std::vector<ProcedureTemplate>& getCustomProcedures();
};

