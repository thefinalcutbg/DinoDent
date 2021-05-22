#include "BridgeController.h"

BridgeController::BridgeController()
{

}

std::vector<std::vector<int>> BridgeController::selectionCutter(const std::vector<int>& indexes)
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

void BridgeController::formatBridges(const std::vector<int>& indexes, std::array<Tooth, 32>* teeth)
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


void BridgeController::formatSelection(const std::vector<int>& selection, std::array<Tooth, 32>*teeth)
{
	for (int i = 0; i < selection.size(); i++)
	{
		if (i == 0){

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

		if (prev_tooth.bridge.position == BridgePos::Begin){
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
