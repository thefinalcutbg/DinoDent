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
        cariesStatus{ CheckState::unchecked }
   //     mobilityStatus{ CheckState::unchecked },
    {}

    std::array<CheckState, Dental::StatusCount> generalStatus{ CheckState::unchecked };
    std::array<CheckState, Dental::SurfaceCount> obturationStatus{ CheckState::unchecked };
    std::array<CheckState, Dental::SurfaceCount> cariesStatus{ CheckState::unchecked };
    std::array<CheckState, Dental::MobilityCount> mobilityStatus{ CheckState::unchecked };
    bool no_data{ true };
};