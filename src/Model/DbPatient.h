#pragma once
#include "Database/sqLite/sqlite3.h"
#include <iostream>
#include "Model/Patient.h"
#include <vector>
#include <map>
#include "Database/database.h"
#include <QDebug>

class DbPatient
{
    char* err;
    sqlite3 *db;
    sqlite3_stmt* stmt;
    int rc;
public:
    DbPatient(Database* database);

    void insert(const Patient& patient);
    void update(const Patient& patient);
    Patient getPatient(std::string patientID);
    void updateAllergies(std::string& patientID, std::string& allergies, std::string& current, std::string& past);
};

