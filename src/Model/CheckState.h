#pragma once
#include <array>
#include <vector>
enum class CheckState { unchecked, checked, partially_checked };

class ToothContainer;

struct CheckModel
{
    CheckModel(const ToothContainer& teeth, const std::vector<int>& selectedTeeth);

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