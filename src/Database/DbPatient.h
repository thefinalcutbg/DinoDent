#pragma once

#include "Model/Patient.h"

namespace DbPatient
{
    void insert(const Patient& patient);
    void update(const Patient& patient);
    Patient getPatient(std::string patientID);
    void updateAllergies(const std::string& patientID, const std::string& allergies, const std::string& current, const std::string& past);
    TeethNotes getPresentNotes(const std::string& patientID);
};

