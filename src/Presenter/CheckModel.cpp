#include "CheckModel.h"

template <size_t Size>
inline void firstIteration(std::array<CheckState, Size>& checkArray, 
					 const std::array<bool, Size>& boolArray)
{
    for (size_t i = 0; i < Size; i++)
		checkArray[i] = static_cast<CheckState>(boolArray[i]);
}

template <size_t Size>
inline void furtherIterations(std::array<CheckState, Size> &checkArray, 
						const std::array<bool, Size>& boolArray)
{

    for (size_t i = 0; i < Size; i++ )
	{
		if (checkArray[i] == CheckState::partially_checked) continue;

		if (static_cast<bool>(checkArray[i]) != boolArray[i])
			checkArray[i] = CheckState::partially_checked;
	}

}

CheckModel::CheckModel(const std::vector<const Tooth*>& selectedTeeth)
	:
	generalStatus{ CheckState::unchecked },
	obturationStatus{ CheckState::unchecked },
	cariesStatus{ CheckState::unchecked },
	mobilityStatus{ CheckState::unchecked }
{
	if (!selectedTeeth.size()) return;

	firstIteration(generalStatus, selectedTeeth[0]->getBoolStatus());
	firstIteration(cariesStatus, selectedTeeth[0]->getCariesBoolStatus());
	firstIteration(obturationStatus, selectedTeeth[0]->getRestorationBoolStatus());
	firstIteration(mobilityStatus, selectedTeeth[0]->getMobilityBoolStatus());

    for (size_t i = 1; i < selectedTeeth.size(); i++)
	{
		furtherIterations(generalStatus, selectedTeeth[i]->getBoolStatus());
		furtherIterations(cariesStatus, selectedTeeth[i]->getCariesBoolStatus());
		furtherIterations(obturationStatus, selectedTeeth[i]->getRestorationBoolStatus());
		furtherIterations(mobilityStatus, selectedTeeth[i]->getMobilityBoolStatus());
	}

    for (size_t i = 1; i < generalStatus.size(); i++) {
		if (generalStatus[i] != CheckState::unchecked) {
			break;
		}
	}

	CheckState& bridge = generalStatus.at(static_cast<int>(Dental::Bridge));

	if (bridge == CheckState::checked && selectedTeeth.size() != 1 &&
		(selectedTeeth[0]->position != Dental::BridgePos::Begin ||
		 selectedTeeth.back()->position != Dental::BridgePos::End))
		bridge = CheckState::partially_checked;

	for (auto& state : generalStatus)
	{
		if (state != CheckState::unchecked) no_data = false;
	}

}

CheckModel::CheckModel(const Tooth& tooth)
{
	firstIteration(generalStatus, tooth.getBoolStatus());
	firstIteration(cariesStatus, tooth.getCariesBoolStatus());
	firstIteration(obturationStatus, tooth.getRestorationBoolStatus());
	firstIteration(mobilityStatus, tooth.getMobilityBoolStatus());

	for (auto& state : generalStatus)
	{
		if (state != CheckState::unchecked) no_data = false;
	}

}
