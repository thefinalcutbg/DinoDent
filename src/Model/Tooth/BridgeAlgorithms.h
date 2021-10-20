#pragma once
#include <vector>
#include <array>
#include <utility>
#include "Tooth.h"

std::vector<std::vector<int> > selectionCutter(const std::vector<int>& indexes)
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


template <auto ptr>
void formatSelection(const std::vector<int>& selection, std::array<Tooth, 32>& teeth)
{

	if (selection.size() == 1) //case in which only one tooth is selected
	{
		auto& singleTooth = teeth[selection[0]].*ptr;
		singleTooth.set(false);
	}

	for (int i = 0; i < selection.size(); i++)
	{
		if (i == 0) {
			(teeth[selection[i]].*ptr).position = BridgePos::Begin;
		}
		else if (i == selection.size() - 1) {
			(teeth[selection[i]].*ptr).position = BridgePos::End;
		}
		else {
			(teeth[selection[i]].*ptr).position = BridgePos::Middle;
		}
	}

	if (selection[0] != 0 && selection[0] != 16)
	{
		auto& prev_tooth = teeth[selection[0] - 1].*ptr;

		if (prev_tooth.position == BridgePos::Begin) {
			prev_tooth.set(false);
		}
		else if (prev_tooth.position == BridgePos::Middle) {
			prev_tooth.position = BridgePos::End;
		}
	}

	if (selection.back() != 15 && selection.back() != 31)
	{
		auto& next_tooth = teeth[selection.back() + 1].*ptr;

		if (next_tooth.position == BridgePos::End) {
			next_tooth.set(false);
		}
		else if (next_tooth.position == BridgePos::Middle) {
			next_tooth.position = BridgePos::Begin;
		}

	}

}



template<auto ptr>
std::pair<int, int> getConstructionRange(const std::array<Tooth, teethCount>& teeth, int tooth_idx)
{
	int begin{ tooth_idx }, end{ tooth_idx };

	auto& current = teeth[tooth_idx].*ptr;

	if (!current.exists())
		return std::make_pair(begin, end);

	for (int i = tooth_idx; i < teethCount; i++) {
		if ((teeth[i].*ptr).position == BridgePos::End) {
			end = i;
			break;
		}
	}

	for (int i = tooth_idx; i >= 0; --i) {
		if ((teeth[i].*ptr).position == BridgePos::Begin) {
			begin = i;
			break;
		}
	}

	return std::make_pair(begin, end);

}
