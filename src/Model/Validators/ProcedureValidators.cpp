#include "ProcedureValidators.h"

bool BridgeRangeValidator::validateInput(int begin, int end)
{
    if ((begin < 16) != (end < 16)){
        return false;
    }

    if (begin == end) {
        return false;
    }
    return true;
}

bool SurfaceValidator::validateInput(const std::array<bool, 6>& surfaces)
{
    for (bool s : surfaces)
    {
        if (s) return true;
    }

    return false;
}


