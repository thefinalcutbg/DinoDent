#pragma once
#include "Database/sqLite/sqlite3.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/Date.h"
#include "Tooth/ToothParser.h"
#include "Database/database.h"

#include <QDebug>

#include <vector>
#include <map>
#include <iostream>


class DbAmbList
{
    char* err;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    ToothParser parser;

    std::string getOlderStatus(std::string patientID);

public:
    DbAmbList(Database* database);

    void insertAmbList(AmbList& ambList, std::string& patientID);
    AmbList* getList(std::string patientID, int currentMonth, int currentYear);

    int getNewNumber(const int& currentYear);

    bool checkExistingAmbNum(const int& currentYear, const int& ambNum);
    std::map<int, bool> getExistingNumbers(const int& currentYear);
    void updateAmbList(AmbList& ambList);
};

