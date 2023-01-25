#pragma once
#include <array>
#include <vector>
#include "Model/Dental/Tooth.h"

enum class CheckState { unchecked, checked, partially_checked };

struct CheckModel
{

    CheckModel(const std::vector<Tooth*>& selectedTeeth);
    CheckModel(const Tooth& tooth);

    CheckModel() :
        generalStatus { CheckState::unchecked },
        obturationStatus{ CheckState::unchecked },
        cariesStatus{ CheckState::unchecked }
   //     mobilityStatus{ CheckState::unchecked },
    {}

    std::array<CheckState, statusCount> generalStatus;
    std::array<CheckState, surfaceCount> obturationStatus;
    std::array<CheckState, surfaceCount> cariesStatus;
    std::array<CheckState, mobilityCount> mobilityStatus;
};