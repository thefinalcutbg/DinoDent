#include "SelectionManager.h"

SelectionManager::SelectionManager() : Teeth(nullptr)
{
}

void SelectionManager::setStatus(std::array<Tooth, 32>* Teeth)
{
    this->Teeth = Teeth;
}

std::vector<Tooth*> SelectionManager::getSelectedTeethPointers(const std::vector<int>& selectedTeethIndexes)
{
    std::vector<Tooth*> selectedTeethPointers;
    selectedTeethPointers.reserve(selectedTeethIndexes.size());

    for (int index : selectedTeethIndexes)
    {
        selectedTeethPointers.emplace_back(&Teeth->at(index));
    }

    return selectedTeethPointers;
}
