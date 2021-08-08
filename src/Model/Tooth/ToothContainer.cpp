#include "ToothContainer.h"
#include <stdexcept>

ToothContainer::ToothContainer(){
    teeth = new std::array<Tooth, teethNum>{};
    for (int i = 0; i < teethNum; i++) teeth->at(i).setIndex(i);
}

ToothContainer::ToothContainer(const ToothContainer& other){
    teeth = new std::array<Tooth, teethNum>;
    for (int i = 0; i < teethNum; i++) teeth->at(i) = other.teeth->at(i);
}

ToothContainer::ToothContainer(ToothContainer&& other) noexcept{
    teeth = other.teeth; other.teeth = nullptr;
}

Tooth& ToothContainer::operator[](int index)
{
    if (teeth == nullptr) throw std::invalid_argument("container has been moved");

    if (index < teethNum || index < 0) throw std::invalid_argument("index out of range");

    return teeth->at(index);
}


ToothContainer::~ToothContainer(){
    if (teeth) delete teeth;
}

std::vector<std::vector<int>> selectionCutter(const std::vector<int>& indexes)
{
	std::vector<std::vector<int>> selections;

	std::vector<int> selection;

	for (int i = 0; i < indexes.size(); i++) {

		selection.emplace_back(indexes[i]);
		if (i + 1 == indexes.size() || indexes[i] + 1 != indexes[i + 1] || indexes[i] == 15)
		{
			selections.emplace_back(selection);
			selection.clear();
		}
	}

	return selections;
}


void ToothContainer::formatSelection(const std::vector<int>& selection)
{
	for (int i = 0; i < selection.size(); i++)
	{
		if (i == 0) {

			teeth->at(selection[i]).bridge.position = BridgePos::Begin;
		}
		else if (i == selection.size() - 1) {
			teeth->at(selection[i]).bridge.position = BridgePos::End;
		}
		else {
			teeth->at(selection[i]).bridge.position = BridgePos::Middle;
		}
	}

	if (selection[0] != 0 && selection[0] != 16)
	{
		Tooth& prev_tooth = teeth->at(selection[0] - 1);

		if (prev_tooth.bridge.position == BridgePos::Begin) {
			prev_tooth.bridge.set(false);
		}
		else if (prev_tooth.bridge.position == BridgePos::Middle) {
			prev_tooth.bridge.position = BridgePos::End;
		}
	}

	if (selection.back() != 15 && selection.back() != 31)
	{
		Tooth& next_tooth = teeth->at(selection.back() + 1);

		if (next_tooth.bridge.position == BridgePos::End) {
			next_tooth.bridge.set(false);
		}
		else if (next_tooth.bridge.position == BridgePos::Middle) {
			next_tooth.bridge.position = BridgePos::Begin;
		}

	}

}


void ToothContainer::formatBridges(const std::vector<int>& indexes)
{
    auto selections = std::move(selectionCutter(indexes));

    for (auto selection : selections)
    {
        if (selection.size() == 1) //case in which only one tooth is selected
        {
            teeth->at(selection[0]).bridge.set(false);
        }

        formatSelection(selection, teeth);
    }
}

void ToothContainer::removeBridge(int tooth_idx)
{
	if (!teeth->at(tooth_idx).bridge.exists()) return;

	for (int i = tooth_idx; i < 32; i++)
	{
		auto& bridge = teeth->at(i).bridge;

		bridge.set(false);

		if (bridge.position == BridgePos::End)
			break;
	}

	for (int i = tooth_idx; i >= 0; --i)
	{
		auto& bridge = teeth->at(i).bridge;

		bridge.set(false);

		if (bridge.position == BridgePos::Begin)
			break;
	}
}
