#pragma once
#include <string>
#include <vector>

#include "Model/Date.h"
#include "Model/Dental/ToothContainer.h"

struct HisSnapshot {
	Date date;
	std::string procedure_name;
	std::string procedure_diagnosis;
	std::string procedure_note;
	FinancingSource financing;
	std::vector<int> affected_teeth;
	ToothContainer teeth;

};