#include "Prescription.h"

std::vector<std::string> Prescription::getMedList() const
{
    std::vector<std::string> result;

    for (auto& m : medicationGroup) {
       
        result.push_back(m.name());
        
        result.back() += u8"; Дозировка: ";

        for (auto d : m.dosage) result.back() += d.parse() +  "; ";
    }

    return result;
}
