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
        restorationStatus{ CheckState::unchecked },
        cariesStatus{ CheckState::unchecked },
        nonCariesStatus{ CheckState::unchecked },
        defRestoStatus{ CheckState::unchecked }
   //     mobilityStatus{ CheckState::unchecked },
    {}

    std::array<CheckState, Dental::StatusCount> generalStatus{ CheckState::unchecked };
    std::array<CheckState, Dental::SurfaceCount> restorationStatus{ CheckState::unchecked };
    std::array<CheckState, Dental::SurfaceCount> cariesStatus{ CheckState::unchecked };
    std::array<CheckState, Dental::SurfaceCount> nonCariesStatus{ CheckState::unchecked };
    std::array<CheckState, Dental::SurfaceCount> defRestoStatus{ CheckState::unchecked };
    std::array<CheckState, Dental::MobilityCount> mobilityStatus{ CheckState::unchecked };
    bool no_data{ true };
};