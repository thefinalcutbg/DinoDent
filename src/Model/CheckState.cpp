#include "CheckState.h"
#include "Tooth/ToothContainer.h"

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

CheckModel::CheckModel(const ToothContainer& teeth, const std::vector<int>& selectedTeeth)
	:
	generalStatus{ CheckState::unchecked },
	obturationStatus{ CheckState::unchecked },
	cariesStatus{ CheckState::unchecked }
{
	if (!selectedTeeth.size()) return;

	//assigning the checkStates to the boolArrays of the first tooth
	firstIteration(generalStatus, teeth[0].getBoolStatus());
	firstIteration(cariesStatus, teeth[0].caries.getBoolStatus());
	firstIteration(obturationStatus, teeth[0].obturation.getBoolStatus());

	if (selectedTeeth.size() == 1) return;

	//further modifying
	for (int i = 1; i < selectedTeeth.size(); i ++)
	{
		furtherIterations(generalStatus, teeth[i].getBoolStatus());
		furtherIterations(cariesStatus, teeth[i].caries.getBoolStatus());
		furtherIterations(obturationStatus, teeth[i].obturation.getBoolStatus());
	}

}