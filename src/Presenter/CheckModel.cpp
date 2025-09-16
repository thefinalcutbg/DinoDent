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
	restorationStatus{ CheckState::unchecked },
	cariesStatus{ CheckState::unchecked },
	nonCariesStatus{ CheckState::unchecked },
	defRestoStatus{ CheckState::unchecked },
	mobilityStatus{ CheckState::unchecked }
{
	if (!selectedTeeth.size()) return;

	firstIteration(generalStatus, selectedTeeth[0]->getBoolStatus());
	firstIteration(cariesStatus, selectedTeeth[0]->getSurfaceBoolStatus(Dental::Caries));
	firstIteration(restorationStatus, selectedTeeth[0]->getSurfaceBoolStatus(Dental::Restoration));
	firstIteration(defRestoStatus, selectedTeeth[0]->getSurfaceBoolStatus(Dental::DefectiveRestoration));
	firstIteration(nonCariesStatus, selectedTeeth[0]->getSurfaceBoolStatus(Dental::NonCariesLesion));
	firstIteration(mobilityStatus, selectedTeeth[0]->getMobilityBoolStatus());

    for (size_t i = 1; i < selectedTeeth.size(); i++)
	{
		furtherIterations(generalStatus, selectedTeeth[i]->getBoolStatus());
		furtherIterations(cariesStatus, selectedTeeth[i]->getSurfaceBoolStatus(Dental::Caries));
		furtherIterations(restorationStatus, selectedTeeth[i]->getSurfaceBoolStatus(Dental::Restoration));
		furtherIterations(defRestoStatus, selectedTeeth[i]->getSurfaceBoolStatus(Dental::DefectiveRestoration));
		furtherIterations(nonCariesStatus, selectedTeeth[i]->getSurfaceBoolStatus(Dental::NonCariesLesion));
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
	firstIteration(cariesStatus, tooth.getSurfaceBoolStatus(Dental::Caries));
	firstIteration(restorationStatus, tooth.getSurfaceBoolStatus(Dental::Restoration));
	firstIteration(defRestoStatus, tooth.getSurfaceBoolStatus(Dental::DefectiveRestoration));
	firstIteration(nonCariesStatus, tooth.getSurfaceBoolStatus(Dental::NonCariesLesion));
	firstIteration(mobilityStatus, tooth.getMobilityBoolStatus());

	for (auto& state : generalStatus)
	{
		if (state != CheckState::unchecked) no_data = false;
	}

}
