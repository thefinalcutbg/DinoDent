#pragma once
#include <string>
#include <vector>

#include "Model/Date.h"
#include "Model/Dental/ToothContainer.h"
#include "Model/Dental/Procedure.h"

struct Procedure;

struct HisSnapshot {

	HisSnapshot(const Procedure& p, const ToothContainer& t);
	HisSnapshot(const ToothContainer& t, Date& date);
	HisSnapshot() {};

	Date date;
	std::string procedure_name;
	std::string procedure_diagnosis;
	std::string procedure_note;
	FinancingSource financing{ FinancingSource::None };
	ToothContainer teeth;
	std::vector<int> affected_teeth;

};
