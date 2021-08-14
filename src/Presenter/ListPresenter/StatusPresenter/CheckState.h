#pragma once
#include <array>
#include <vector>
#include "Model/Tooth/Tooth.h"

enum class CheckState { unchecked, checked, partially_checked };

class ToothContainer;

struct CheckModel
{

    CheckModel(const std::vector<Tooth*>& selectedTeeth);

    CheckModel() :
        generalStatus { CheckState::unchecked },
        obturationStatus{ CheckState::unchecked },
        cariesStatus{ CheckState::unchecked }
    {}

    CheckModel(
        std::array<CheckState, 18> generalStatus,
        std::array<CheckState, 6> obturationStatus,
        std::array<CheckState, 6> cariesStatus
    ) :
        generalStatus(generalStatus),
        obturationStatus(obturationStatus),
        cariesStatus(cariesStatus)
    {}

    std::array<CheckState, 18> generalStatus;
    std::array<CheckState, 6> obturationStatus;
    std::array<CheckState, 6> cariesStatus;
};