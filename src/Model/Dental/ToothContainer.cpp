#include "ToothContainer.h"
#include <stdexcept>
#include <utility>
#include "BridgeAlgorithms.h"

constexpr int defaultSurfaces[32] = { 0,0,0,0,0,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,0,0,0,0,0 };

ToothContainer::ToothContainer(){

	teeth.reserve(teethCount);
	for (int i = 0; i < teethCount; i++)
	{
		teeth.emplace_back();
		teeth.back().setIndex(i);
		teeth.back().caries.setDefaultSurface(defaultSurfaces[i]);
		teeth.back().obturation.setDefaultSurface(defaultSurfaces[i]);
	}
}

const Tooth& ToothContainer::operator[](int index) const
{
	if (teeth.empty()) throw std::invalid_argument("container has been moved");

	if (index >= teethCount || index < 0) throw std::invalid_argument("index out of range");

	return teeth.at(index);
}


Tooth& ToothContainer::operator[](int index)
{
  
    if (index >= teethCount || index < 0) throw std::invalid_argument("index out of range");

    return teeth.at(index);
}

int ToothContainer::getMissingTeethCount(bool countWisdom) const
{
	int missingTeeth{ 0 };

	constexpr int wisdomTeethIdx[4]{ 0, 15, 16, 31 };

	for (const auto& t : teeth)
	{
		for (auto& i : wisdomTeethIdx)
			if (!countWisdom && t.index == i)
				continue;

		if (t.extraction.exists() || t.implant.exists())
			missingTeeth++;
	}

	return missingTeeth;
}



std::vector<Tooth*> ToothContainer::getSelectedTeethPtr(std::vector<int> selectedIndexes)
{
	std::vector<Tooth*> selectedPtr;
	selectedPtr.reserve(selectedIndexes.size());
	for (auto i : selectedIndexes)
		selectedPtr.push_back(&teeth.at(i));
	return selectedPtr;
}


void ToothContainer::formatBridges(const std::vector<int>& indexes)
{
    auto selections = std::move(selectionCutter(indexes));

    for (auto selection : selections)
	{
        formatSelection<&Tooth::bridge>(selection, teeth);
		formatSelection<&Tooth::splint>(selection, teeth);
    }
}


void ToothContainer::removeBridge(int tooth_idx)
{
	auto [bridgeBegin, bridgeEnd] = getConstructionRange<&Tooth::bridge>(teeth, tooth_idx);

	for (int i = bridgeBegin; i <= bridgeEnd; i++)
		teeth.at(i).bridge.set(false);

}


void ToothContainer::setToothDetails(const Tooth& tooth)
{
	int idx = tooth.index;

	auto [bridgeBegin, bridgeEnd] = getConstructionRange<&Tooth::bridge>(teeth, idx);

	auto& bridgeStat = tooth.bridge;

	for (int i = bridgeBegin; i <= bridgeEnd; i++)
	{
		auto& bridge = teeth.at(i).bridge;
		bridge.set(bridgeStat.exists());
		bridge.data = bridgeStat.data;
		bridge.LPK = bridgeStat.LPK;
	}

	auto& splitStat = tooth.splint;

	auto [splintBegin, splintEnd] = getConstructionRange<&Tooth::splint>(teeth, idx);

	for (int i = splintBegin; i <= splintEnd; i++)
	{
		auto& splint = teeth.at(i).splint;
		splint.set(splitStat.exists());
		splint.LPK = splitStat.LPK;
	}

	teeth.at(idx) = tooth;

}