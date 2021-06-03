#pragma once
#include <vector>
#include "ManipulationTemplate.h"

class CustomProcedures
{
	static CustomProcedures _instance;
	std::vector<ManipulationTemplate> _customProcedures;

	CustomProcedures();

public:
	CustomProcedures(const CustomProcedures&) = delete;
	static CustomProcedures& instance();

	void loadCustomProcedures();
	const std::vector<ManipulationTemplate>& getCustomProcedures();
};

