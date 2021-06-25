#pragma once

#include "AbstractORM.h"
#include <iostream>
#include "Model/Patient.h"
#include <vector>
#include <map>
#include <QDebug>

class DbPatient : public AbstractORM
{


public:
    DbPatient();

    void insert(const Patient& patient);
    void update(const Patient& patient);
    Patient getPatient(std::string patientID);
    void updateAllergies(const std::string& patientID, const std::string& allergies, const std::string& current, const std::string& past);
};

