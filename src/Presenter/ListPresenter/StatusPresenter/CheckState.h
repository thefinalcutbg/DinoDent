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

    std::array<CheckState, statusCount> generalStatus;
    std::array<CheckState, surfaceCount> obturationStatus;
    std::array<CheckState, surfaceCount> cariesStatus;
};