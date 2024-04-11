#include "Prescription.h"

std::vector<std::string> Prescription::getMedList() const
{
    std::vector<std::string> result;

    for (auto& m : medicationGroup) {
       
        result.push_back(m.name() + "; " + m.parseFullDosage());
    }

    return result;
}
