#pragma once

#include <vector>
#include <string>

#include "Model/Dental/ToothContainer.h"

struct HISTooth {
	ToothIndex idx;
	std::vector<std::string> conditions;
};

struct HISProcedureResult : public std::vector<HISTooth>{

	ToothContainer getToothContainer() const;
	std::string getToothString() const;
	std::vector<const Tooth*> applyProcedure(ToothContainer& teeth) const;
	std::vector<int> getArrayIndexes() const;
};