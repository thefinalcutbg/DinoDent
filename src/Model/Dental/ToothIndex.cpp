#include "ToothIndex.h"
#include "ToothUtils.h"

std::string ToothIndex::getNhifNumenclature() const
{
    return ToothUtils::getNhifNumber(index, temp, supernumeral);
}
