#pragma once
#include <array>
#include <vector>
#include "Model/Tooth/Tooth.h"

enum class CheckState { unchecked, checked, partially_checked };

class ToothContainer;

struct CheckModel
{

    CheckModel(const std::vector<Tooth*>& selectedTeeth);
    CheckModel(const Tooth& tooth);

    CheckModel() :
        generalStatus { CheckState::unchecked },
        obturationStatus{ CheckState::unchecked },
        cariesStatus{ CheckState::unchecked }
    {}

    std::array<CheckState, 18> generalStatus;
    std::array<CheckState, 6> obturationStatus;
    std::array<CheckState, 6> cariesStatus;
};