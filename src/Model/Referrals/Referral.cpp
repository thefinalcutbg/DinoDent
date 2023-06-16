#include "Referral.h"
#include "Model/User.h"
#include "Model/FreeFunctions.h"

std::string Referral::getNumber() const
{
    if (!isNrnType()) {
        return FreeFn::leadZeroes(number, 12);
    }

    return nrn;
}
