#pragma once
#include "Libraries/sqLite/sqlite3.h"
#include <iostream>
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include <vector>
#include <map>

class Database
{
    char* err;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

public:
    Database();
    ~Database(){ sqlite3_close_v2(db); }

};

