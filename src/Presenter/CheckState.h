#pragma once
#include <array>
#include <vector>
#include "Model/Dental/Tooth.h"

enum class CheckState { unchecked, checked, partially_checked };

struct CheckModel
{

    CheckModel(const std::vector<const Tooth*>& selectedTeeth);
    CheckModel(const Tooth& tooth);

    CheckModel() :
        generalStatus{ CheckState::unchecked },
        obturationStatus{ CheckState::unchecked },
        cariesStatus{ CheckState::unchecked },
        isHealthy { true }
   //     mobilityStatus{ CheckState::unchecked },
    {}

    std::array<CheckState, statusCount> generalStatus{ CheckState::unchecked };
    std::array<CheckState, surfaceCount> obturationStatus{ CheckState::unchecked };
    std::array<CheckState, surfaceCount> cariesStatus{ CheckState::unchecked };
    std::array<CheckState, mobilityCount> mobilityStatus{ CheckState::unchecked };
    bool isHealthy{ true };
};