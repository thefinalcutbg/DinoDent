#pragma once
#include "sqLite/sqlite3.h"
#include <string>

class AbstractORM
{
public:
    AbstractORM() :
        err(nullptr),
        db(nullptr),
        stmt(nullptr),
        rc(0)
    {}

protected:
    char* err;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    inline void openConnection()
    {
        int i =  sqlite3_open("DATATEST.db", &db); 
        if (db == nullptr) throw("db open error");

    };

    inline void closeConnection()
    {
        sqlite3_close_v2(db);
        db = nullptr;
     
    }
};

