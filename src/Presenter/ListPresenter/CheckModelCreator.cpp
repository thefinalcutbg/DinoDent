#include "CheckModelCreator.h"

std::array<bool, 18> CheckModelCreator::generalStatusBool(const Tooth& tooth)
{

    return std::array<bool, 18>{
            tooth.temporary.exists(),
            tooth.obturation.exists(),
            tooth.caries.exists(),
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

    return std::array<bool, 6> {
            tooth.obturation.exists(0),
            tooth.obturation.exists(1),
            tooth.obturation.exists(2),
            tooth.obturation.exists(3),
            tooth.obturation.exists(4),
            tooth.obturation.exists(5)
    };
}

std::array<bool, 6> CheckModelCreator::cariesStatusBool(const Tooth& tooth)
{
    return std::array<bool, 6> {
            tooth.caries.exists(0),
            tooth.caries.exists(1),
            tooth.caries.exists(2),
            tooth.caries.exists(3),
            tooth.caries.exists(4),
            tooth.caries.exists(5)
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

void CheckModelCreator::refreshModel(const std::vector<Tooth*>& selectedTeeth)
{
    //generating models:
     CheckModel model(
                        getModelArray(selectedTeeth, &CheckModelCreator::generalStatusBool),
                        getModelArray(selectedTeeth, &CheckModelCreator::obturationStatusBool),
                        getModelArray(selectedTeeth, &CheckModelCreator::cariesStatusBool)
    );

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
