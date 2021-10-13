#include "ToothContainer.h"
#include <stdexcept>
#include <utility>
#include "BridgeAlgorithms.h"

constexpr int defaultSurfaces[32] = { 0,0,0,0,0,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,0,0,0,0,0 };

ToothContainer::ToothContainer(){
    teeth = new std::array<Tooth, teethCount>{};
	for (int i = 0; i < teethCount; i++)
	{
		teeth->at(i).setIndex(i);
		teeth->at(i).caries.setDefaultSurface(defaultSurfaces[i]);
		teeth->at(i).obturation.setDefaultSurface(defaultSurfaces[i]);
	}
}

ToothContainer::ToothContainer(const ToothContainer& other){
    teeth = new std::array<Tooth, teethCount>;
    for (int i = 0; i < teethCount; i++) teeth->at(i) = other.teeth->at(i);
}

ToothContainer::ToothContainer(ToothContainer&& other) noexcept{
    teeth = other.teeth; other.teeth = nullptr;
}

Tooth& ToothContainer::operator[](int index)
{

    if (teeth == nullptr) throw std::invalid_argument("container has been moved");

    if (index >= teethCount || index < 0) throw std::invalid_argument("index out of range");

    return teeth->at(index);
}

const Tooth& ToothContainer::operator[](int index) const
{
	if (teeth == nullptr) throw std::invalid_argument("container has been moved");

	if (index >= teethCount || index < 0) throw std::invalid_argument("index out of range");

	return teeth->at(index);
}


std::vector<Tooth*> ToothContainer::getSelectedTeethPtr(std::vector<int> selectedIndexes)
{
	std::vector<Tooth*> selectedPtr;
	selectedPtr.reserve(selectedIndexes.size());
	for (auto i : selectedIndexes)
		selectedPtr.push_back(&teeth->at(i));
	return selectedPtr;
}

ToothContainer& ToothContainer::operator=(const ToothContainer& other)
{
	if (this == &other) return *this;

	delete teeth;
	teeth = new std::array<Tooth, teethCount>;
	for (int i = 0; i < teethCount; i++) teeth->at(i) = other.teeth->at(i);
	return *this;
}

ToothContainer::~ToothContainer(){
    if (teeth) delete teeth;
}


void ToothContainer::formatBridges(const std::vector<int>& indexes)
{
    auto selections = std::move(selectionCutter(indexes));

    for (auto selection : selections)
	{
        formatSelection<&Tooth::bridge>(selection, *teeth);
		formatSelection<&Tooth::splint>(selection, *teeth);
    }
}


void ToothContainer::removeBridge(int tooth_idx)
{
	auto [bridgeBegin, bridgeEnd] = getConstructionRange<&Tooth::bridge>(*teeth, tooth_idx);

	for (int i = bridgeBegin; i <= bridgeEnd; i++)
		teeth->at(i).bridge.set(false);

}


void ToothContainer::setToothDetails(const Tooth& tooth)
{
	int idx = tooth.index;

	auto [bridgeBegin, bridgeEnd] = getConstructionRange<&Tooth::bridge>(*teeth, idx);

	auto& bridgeStat = tooth.bridge;

	for (int i = bridgeBegin; i <= bridgeEnd; i++)
	{
		auto& bridge = teeth->at(i).bridge;
		bridge.set(bridgeStat.exists());
		bridge.data = bridgeStat.data;
		bridge.LPK = bridgeStat.LPK;
	}

	auto& splitStat = tooth.splint;

	auto [splintBegin, splintEnd] = getConstructionRange<&Tooth::splint>(*teeth, idx);

	for (int i = splintBegin; i <= splintEnd; i++)
	{
		auto& splint = teeth->at(i).splint;
		splint.set(splitStat.exists());
		splint.LPK = splitStat.LPK;
	}

	teeth->at(idx) = tooth;

}
