#include "ToothIndex.h"
#include "ToothUtils.h"

std::string ToothIndex::getNhifNumenclature() const
{
    if (!isValid()) return "";
    return ToothUtils::getNhifNumber(*this);
}
