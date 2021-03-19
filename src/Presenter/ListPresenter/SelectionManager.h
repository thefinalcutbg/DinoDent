#pragma once
#include <vector>
#include <array>
#include "Model/Tooth/Tooth.h"

class SelectionManager
{
	

	std::array<Tooth, 32>* Teeth;

public:
	SelectionManager();
	void setStatus(std::array<Tooth, 32>* Teeth);
	std::vector<Tooth*> getSelectedTeethPointers(std::vector<int>& selectedTeethIndexes);

};

