#include "SelectionManager.h"

SelectionManager::SelectionManager() : Teeth(NULL)
{
}

void SelectionManager::setStatus(std::array<Tooth, 32>* Teeth)
{
	this->Teeth = Teeth;
}

std::vector<Tooth*> SelectionManager::getSelectedTeethPointers(std::vector<int>& selectedTeethIndexes)
{
	std::vector<Tooth*> selectedTeethPointers;
	selectedTeethPointers.reserve(selectedTeethIndexes.size());

	for (int index : selectedTeethIndexes)
	{
		selectedTeethPointers.emplace_back(&Teeth->at(index));
	}

	return selectedTeethPointers;
}
