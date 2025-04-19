#include "ToothContainer.h"
#include <stdexcept>
#include <utility>
#include "BridgeAlgorithms.h"
#include "Dental.h"
#include <optional>

using namespace Dental;

ToothContainer::ToothContainer() {

	teeth.reserve(Dental::teethCount);

	for (int i = 0; i < teethCount; i++)
	{
		teeth.emplace_back(i);
	}
}

const Tooth& ToothContainer::operator[](int index) const
{
	if (teeth.empty()) throw std::invalid_argument("container has been moved");

	if (index >= teethCount || index < 0) throw std::invalid_argument("index out of range");

    return teeth.at(index);
}

bool ToothContainer::noData() const
{
    for(auto& t : teeth){
        if(!t.noData()) return false;
    }

    return true;
}


Tooth& ToothContainer::operator[](int index)
{

	if (index >= teethCount || index < 0) throw std::invalid_argument("index out of range");

	return teeth.at(index);
}

int ToothContainer::getMissingTeethCount(bool countWisdom) const
{
	int missingTeeth{ 0 };

	for (const auto& t : teeth)
	{
        if (!countWisdom && t.isWisdom())
            continue;

		if (t[Missing] || t[Implant])
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
		selectedPtr.push_back(&teeth.at(i).getSupernumeral());
	}

	return selectedPtr;
}

void ToothContainer::copyFromHis(const ToothContainer& other)
{

	std::optional<Dental::BridgePos> position;

	for (int i = 0; i < teethCount; i++)
	{
		if (other[i].noData()) continue;

		if (teeth[i][Bridge] && other[i][Bridge]) {

			position = teeth[i].position;
		}
		else if (other[i][Bridge]) {
			position = other[i].position;
		}
		else {
			position.reset();
		}

		teeth[i].copyFromHIS(other[i]);

		if (position) {
			teeth[i].position = position.value();
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
		status == Healthy ||
		status == Missing ||
		status == Impacted ||
		status == Root ||
		status == Bridge ||
		status == Splint
		;
}


bool ToothContainer::needsBridgeFormatting(int status)
{
	return
		status == Bridge ||
		status == Crown ||
		status == Splint ||
		status == Denture ||
		status == Healthy
		;
}

void ToothContainer::setStatus(const std::vector<int>& selectedIndexes, Dental::StatusType t, int code, bool state, bool supernumeral)
{

	//general status optimization:
	if (state &&
		selectedIndexes.size() == 1 &&
		t == Dental::StatusType::General &&
		(code == Bridge || code == Splint)
		)
	{
		return;
	}

	for (auto& idx : selectedIndexes)
	{
		auto& tooth = supernumeral ? teeth[idx].getSupernumeral() : teeth[idx];
		tooth.setStatus(t, code, state);
	}

	if (t != Dental::StatusType::General) return;

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
		teeth[idx].clearStatuses();
	}
	
	formatBridges(selectedTeethidx);

}


void ToothContainer::formatBridges(const std::vector<int>& indexes)
{
	auto selections = std::move(selectionCutter(indexes));

	for (auto& selection : selections)
	{
		formatSelection(selection, teeth, Dental::Bridge);
		formatSelection(selection, teeth, Dental::Splint);
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
		if (!teeth[i][Bridge]) continue;

		if (teeth[i].position == BridgePos::Begin) constructionBegin = i;
		else if (teeth[i].position == BridgePos::End) {
			constructions.push_back(std::make_pair(constructionBegin, i));
			constructionBegin = -1;
		}

	}

	//getting splints
	for (int i = 0; i < 32; i++)
	{
		if (!teeth[i][Splint]) continue;

		if (teeth[i].position == BridgePos::Begin) constructionBegin = i;
		else if (teeth[i].position == BridgePos::End) {
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
				teeth[i].setStatus(Bridge, false);
				teeth[i].setStatus(Splint, false);
			}
		}

	}
}


void ToothContainer::removeBridgeOrSplint(const std::vector<int>& selectedIndexes)
{
	for (auto index : selectedIndexes) {
		auto [bridgeBegin, bridgeEnd] = getConstructionRange(teeth, index, Bridge);

		for (int i = bridgeBegin; i <= bridgeEnd; i++)
			teeth.at(i).setStatus(Bridge, false);
	}

	for (auto index : selectedIndexes) {
		auto [bridgeBegin, bridgeEnd] = getConstructionRange(teeth, index, Splint);

		for (int i = bridgeBegin; i <= bridgeEnd; i++)
			teeth.at(i).setStatus(Splint, false);
	}

	formatBridges(selectedIndexes);


}

void ToothContainer::removeBridgeOrSplint(int begin, int end)
{
	for (int i = begin; i <= end; i++) {
		teeth[i].setStatus(Bridge, false);
		teeth[i].setStatus(Splint, false);
	}

	if (begin != 0)
	{
		auto& prevTooth = teeth.at(begin - 1);

		auto shouldBeCrown =
			prevTooth.position == BridgePos::Begin &&
			prevTooth.canHaveACrown()
			;

		if (shouldBeCrown) prevTooth.setStatus(Crown, true);
	}

	if (end != teeth.size() - 1)
	{
		auto& nextTooth = teeth.at(end + 1);

		auto shouldBeCrown =
			nextTooth.position == BridgePos::End &&
			nextTooth.canHaveACrown()
			;

		if (shouldBeCrown) nextTooth.setStatus(Crown, true);
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
	int idx = tooth.index();

	auto [bridgeBegin, bridgeEnd] = getConstructionRange(teeth, idx, Bridge);

	for (int i = bridgeBegin; i <= bridgeEnd; i++)
	{
		teeth.at(i).setStatus(Bridge, tooth[Bridge]);
		teeth.at(i).setLPK(
			Bridge,
			tooth.getLPK(Bridge)
		);
	}

	auto [splintBegin, splintEnd] = getConstructionRange(teeth, idx, Splint);

	for (int i = splintBegin; i <= splintEnd; i++)
	{
		teeth.at(i).setStatus(Splint, tooth[Splint]);
		teeth.at(i).setLPK(
			Splint,
			tooth.getLPK(Splint)
		);
	}

	teeth.at(idx) = tooth;

}
