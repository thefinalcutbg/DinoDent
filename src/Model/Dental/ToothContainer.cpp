#include "ToothContainer.h"
#include <stdexcept>
#include <utility>
#include "BridgeAlgorithms.h"

constexpr int defaultSurfaces[32] = { 0,0,0,0,0,3,3,3,3,3,3,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,0,0,0,0,0 };



ToothContainer::ToothContainer(){

	teeth.reserve(teethCount);
	for (int i = 0; i < teethCount; i++)
	{
		teeth.emplace_back(i);
		teeth.back().caries.setDefaultSurface(defaultSurfaces[i]);
		teeth.back().obturation.setDefaultSurface(defaultSurfaces[i]);
		teeth.back().dsn.init(i);
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

std::vector<const Tooth*> ToothContainer::getSelectedTeethPtr(std::vector<int> selectedIndexes) const
{
	std::vector<const Tooth*> selectedPtr;
	selectedPtr.reserve(selectedIndexes.size());
	for (auto i : selectedIndexes)
		selectedPtr.push_back(&teeth.at(i));
	return selectedPtr;
}

std::vector<const Tooth*> ToothContainer::getSelectedDsnPtr(std::vector<int> selectedIndexes) const
{
	std::vector<const Tooth*> selectedPtr;

	selectedPtr.reserve(selectedIndexes.size());

	for (auto i : selectedIndexes)
	{
		selectedPtr.push_back(&teeth.at(i).dsn.tooth());
	}

	return selectedPtr;
}

void ToothContainer::copyFromHis(const ToothContainer& other)
{

	std::optional<BridgePos> position;

	for (int i = 0; i < teethCount; i++)
	{
		if (other[i].noData()) continue;

		if (teeth[i].bridge && other[i].bridge) {

			position = teeth[i].bridge.position;
		}
		else {
			position.reset();
		}

		teeth[i].copyFromHIS(other[i]);

		if (position) {
			teeth[i].bridge.position = position.value();
		}
	}
}

void ToothContainer::copyOnlyOnUnknown(const ToothContainer& other)
{
	for (int i = 0; i < teethCount; i++)
	{
		if (this->at(i).noData()) {
			this->at(i) = other.at(i);
		}
	}
}

bool ToothContainer::canResultInNonRetainedConstruction(int status)
{
	return	
		status == StatusCode::Healthy ||
		status == StatusCode::Extraction ||
		status == StatusCode::Impacted ||
		status == StatusCode::Root ||
		status == StatusCode::Bridge ||
		status == StatusCode::FiberSplint
		;
}


bool ToothContainer::needsBridgeFormatting(int status)
{
	return
		status == StatusCode::Bridge ||
		status == StatusCode::Crown ||
		status == StatusCode::FiberSplint ||
		status == StatusCode::Denture ||
		status == StatusCode::Healthy
		;
}

void ToothContainer::setStatus(const std::vector<int>& selectedIndexes, StatusType t, int code, bool state, bool dsn)
{
	//hyperdontic
	if (dsn)
	{
		for (auto idx : selectedIndexes)
		{
			teeth[idx].dsn.tooth().setStatus(t, code, state);
			teeth[idx].setStatus(StatusCode::Dsn, !teeth[idx].dsn.tooth().noData());
		}

		return;
	}

	//not general status
	if (t != StatusType::general)
	{
		for (auto idx : selectedIndexes)
		{
			teeth[idx].setStatus(t, code, state);
		}

		return;
	}

	//general status:
	if (state &&
		selectedIndexes.size() == 1 &&
		(code == StatusCode::Bridge || code == StatusCode::FiberSplint)
		)
	{
		return;
	}

	for (auto& idx : selectedIndexes)
	{
		teeth[idx].setStatus(StatusType::general, code, state);

		if (code == StatusCode::Dsn) {

			if (state && teeth[idx].dsn.tooth().noData()) {
				teeth[idx].dsn.tooth().setStatus(StatusCode::Healthy);
			}
			else if (!state)
			{
				teeth[idx].dsn.tooth().removeStatus();
			}
		}
	}

	if (needsBridgeFormatting(code)) {
		formatBridges(selectedIndexes);
	}
	else if (canResultInNonRetainedConstruction(code))
	{
		removeNonRetainedConstructions();
	}
}

void ToothContainer::removeEveryStatus(const std::vector<int>& selectedTeethidx)
{
	for (auto idx : selectedTeethidx)
	{
		teeth[idx].removeStatus();
	}

	formatBridges(selectedTeethidx);
}


void ToothContainer::formatBridges(const std::vector<int>& indexes)
{
    auto selections = std::move(selectionCutter(indexes));

    for (auto& selection : selections)
	{
       formatSelection<&Tooth::bridge>(selection, teeth);
	   formatSelection<&Tooth::splint>(selection, teeth);

    }

	removeNonRetainedConstructions();
}

void ToothContainer::removeNonRetainedConstructions()
{
	std::vector<std::pair<int, int>> constructions;

	int constructionBegin = -1;

	//getting bridges
	for (int i = 0; i < 32; i++)
	{
		if (!teeth[i].bridge) continue;

		if (teeth[i].bridge.position == BridgePos::Begin) constructionBegin = i;
		else if (teeth[i].bridge.position == BridgePos::End) {
			constructions.push_back(std::make_pair(constructionBegin, i));
			constructionBegin = -1;
		}

	}

	//getting splints
	for (int i = 0; i < 32; i++)
	{
		if (!teeth[i].splint) continue;

		if (teeth[i].splint.position == BridgePos::Begin) constructionBegin = i;
		else if (teeth[i].splint.position == BridgePos::End) {
			constructions.push_back(std::make_pair(constructionBegin, i));
			constructionBegin = -1;
		}

	}
	
	//removing those, who doesn't have at least one retainer
	for (auto& pair : constructions)
	{
		bool noRetainers = true;

		if (pair.first == -1) continue;

		for (int i = pair.first; i <= pair.second; i++)
		{
			if (teeth[i].canHaveACrown()) {
				noRetainers = false;
				break;
			}
		}

		if (noRetainers) {
			for (int i = pair.first; i <= pair.second; i++) {
				teeth[i].setStatus(StatusCode::Bridge, false);
				teeth[i].setStatus(StatusCode::FiberSplint, false);
			}
		}

	}
}


void ToothContainer::removeBridgeOrSplint(const std::vector<int>& selectedIndexes)
{
	for (auto index : selectedIndexes) {
		auto [bridgeBegin, bridgeEnd] = getConstructionRange<&Tooth::bridge>(teeth, index);

		for (int i = bridgeBegin; i <= bridgeEnd; i++)
			teeth.at(i).setStatus(StatusCode::Bridge, false);
	}

	for (auto index : selectedIndexes) {
		auto [bridgeBegin, bridgeEnd] = getConstructionRange<&Tooth::splint>(teeth, index);

		for (int i = bridgeBegin; i <= bridgeEnd; i++)
			teeth.at(i).setStatus(StatusCode::FiberSplint, false);
	}

	formatBridges(selectedIndexes);
	

}

void ToothContainer::removeBridgeOrSplint(int begin, int end)
{
	for (int i = begin; i <= end; i++) {
		teeth[i].removeStatus(StatusCode::Bridge);
		teeth[i].removeStatus(StatusCode::FiberSplint);
	}

	if (begin != 0)
	{
		auto& prevTooth = teeth.at(begin - 1);

		auto shouldBeCrown =
			prevTooth.bridge.position == BridgePos::Begin &&
			prevTooth.canHaveACrown()
		;

		if (shouldBeCrown) prevTooth.crown.set(true);
	}

	if (end != teeth.size() - 1)
	{
		auto& nextTooth = teeth.at(end + 1);

		auto shouldBeCrown =
			nextTooth.bridge.position == BridgePos::End &&
			nextTooth.canHaveACrown()
			;

		if (shouldBeCrown) nextTooth.setStatus(StatusCode::Crown, true);
	}

	std::vector<int> selection;

	for (auto i = begin; i <= end; i++)
	{
		selection.push_back(i);
	}

	formatBridges(selection);

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
