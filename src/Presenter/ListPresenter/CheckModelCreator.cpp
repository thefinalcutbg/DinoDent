#include "CheckModelCreator.h"

std::array<bool, 18> CheckModelCreator::generalStatusBool(const Tooth& tooth)
{
	return std::array<bool, 18>{
			tooth.temporary.exists(),
			tooth.obturation,
			tooth.caries,
			tooth.pulpitis.exists(),
			tooth.lesion.exists(),
			tooth.endo.exists(),
			tooth.post.exists(),
			tooth.root.exists(),
			tooth.fracture.exists(),
			tooth.extraction.exists(),
			tooth.periodontitis.exists(),
			tooth.mobility.exists()&& tooth.mobility.degree == Degree::First,
			tooth.mobility.exists()&& tooth.mobility.degree == Degree::Second,
			tooth.mobility.exists()&& tooth.mobility.degree == Degree::Third,
			tooth.crown.exists(),
			tooth.bridge.exists(),
			tooth.implant.exists(),
			tooth.hyperdontic.exists(),
	};
}

std::array<bool, 6> CheckModelCreator::obturationStatusBool(const Tooth& tooth)
{
	if (!tooth.obturation) {
		return std::array<bool, 6>{0,0,0,0,0,0};
	}

	return std::array<bool, 6> {
			tooth.o_surf[0].exists(),
			tooth.o_surf[1].exists(),
			tooth.o_surf[2].exists(),
			tooth.o_surf[3].exists(),
			tooth.o_surf[4].exists(),
			tooth.o_surf[5].exists()
	};
}

std::array<bool, 6> CheckModelCreator::cariesStatusBool(const Tooth& tooth)
{
	if (!tooth.caries) {
		return std::array<bool, 6>{0,0,0,0,0,0};
	}

	return std::array<bool, 6> {
			tooth.c_surf[0].exists(),
			tooth.c_surf[1].exists(),
			tooth.c_surf[2].exists(),
			tooth.c_surf[3].exists(),
			tooth.c_surf[4].exists(),
			tooth.c_surf[5].exists()
	};
}

CheckModelCreator::CheckModelCreator()
{
	recievers.reserve(2);
}

void CheckModelCreator::addReciever(CheckModelReciever* reciever)
{
	recievers.emplace_back(reciever);
}

void CheckModelCreator::refreshModel(std::vector<Tooth*>& selectedTeeth)
{
	//generating models:
	 CheckModel model{
						getModelArray(selectedTeeth, CheckModelCreator::generalStatusBool),
						getModelArray(selectedTeeth, CheckModelCreator::obturationStatusBool),
						getModelArray(selectedTeeth, CheckModelCreator::cariesStatusBool)
	};

	 //edge case for bridge
	 CheckState& bridge = model.generalStatus.at(static_cast<int>(StatusCode::Bridge));

	 if (
		 bridge == CheckState::checked && selectedTeeth.size() != 1 &&
			 (
				selectedTeeth.at(0)->bridge.position != BridgePos::Begin ||
				selectedTeeth.back()->bridge.position != BridgePos::End
			 )
		 ) 
	 {
		 bridge = CheckState::partially_checked;
	 }

	 //updating models of the recievers:
	 for (CheckModelReciever* reciever : recievers) {
		 reciever->setCheckModel(model);
	 }
}

template<int Size>
std::array<CheckState, Size> CheckModelCreator::getModelArray(std::vector<Tooth*>& teeth, std::array<bool, Size>(*function)(const Tooth& tooth))
{

	std::array<CheckState, Size> checkArray{ CheckState::unchecked };

	for (int toothNum = 0; toothNum < teeth.size(); toothNum++)
	{
		std::array<bool, Size> boolArray = (function(*teeth[toothNum]));

		if (toothNum == 0) //first iteration;
		{
			for (int i = 0; i < Size; i++)
			{
				if (boolArray[i]) {
					checkArray[i] = CheckState::checked;
				}
			}
			continue;
		}

		for (int i = 0; i < Size; i++) //further iterations;
		{
			if (checkArray[i] == CheckState::partially_checked) {
				continue;
			}
			else if (int(checkArray[i]) != boolArray[i]) {
				checkArray[i] = CheckState::partially_checked;
			}

		}
	}

	return checkArray;
}