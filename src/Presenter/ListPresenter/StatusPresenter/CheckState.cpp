#include "CheckState.h"
#include "Model/Tooth/ToothContainer.h"

template <size_t Size>
inline void firstIteration(std::array<CheckState, Size>& checkArray, 
					 const std::array<bool, Size>& boolArray)
{
	for (int i = 0; i < Size; i++)
		checkArray[i] = static_cast<CheckState>(boolArray[i]);
}

template <size_t Size>
inline void furtherIterations(std::array<CheckState, Size> &checkArray, 
						const std::array<bool, Size>& boolArray)
{

	for (int i = 0; i < Size; i++ )
	{
		if (checkArray[i] == CheckState::partially_checked) continue;

		if (static_cast<bool>(checkArray[i]) != boolArray[i])
			checkArray[i] = CheckState::partially_checked;
	}

}

CheckModel::CheckModel(const std::vector<Tooth*>& selectedTeeth)
	:
	generalStatus{ CheckState::unchecked },
	obturationStatus{ CheckState::unchecked },
	cariesStatus{ CheckState::unchecked }
{
	if (!selectedTeeth.size()) return;

	firstIteration(generalStatus, selectedTeeth[0]->getBoolStatus());
	firstIteration(cariesStatus, selectedTeeth[0]->caries.getBoolStatus());
	firstIteration(obturationStatus, selectedTeeth[0]->obturation.getBoolStatus());

	if (selectedTeeth.size() == 1) return;

	for (int i = 1; i < selectedTeeth.size(); i++)
	{
		furtherIterations(generalStatus, selectedTeeth[i]->getBoolStatus());
		furtherIterations(cariesStatus, selectedTeeth[i]->caries.getBoolStatus());
		furtherIterations(obturationStatus, selectedTeeth[i]->obturation.getBoolStatus());
	}

	CheckState& bridge = generalStatus.at(static_cast<int>(StatusCode::Bridge));

	if (bridge == CheckState::checked && selectedTeeth.size() != 1 &&
		(selectedTeeth[0]->bridge.position != BridgePos::Begin ||
		 selectedTeeth.back()->bridge.position != BridgePos::End))
		bridge = CheckState::partially_checked;

}

CheckModel::CheckModel(const Tooth& tooth)
{
	firstIteration(generalStatus, tooth.getBoolStatus());
	firstIteration(cariesStatus, tooth.caries.getBoolStatus());
	firstIteration(obturationStatus, tooth.obturation.getBoolStatus());
}
