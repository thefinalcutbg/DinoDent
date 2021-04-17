#pragma once
#include "Database/sqLite/sqlite3.h"

class AbstractORM
{
public:
    AbstractORM() : 
        db(NULL),
        err(NULL),
        stmt(NULL),
        rc(0)
    {}

protected:
    char* err;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    void openConnection()
    {
        sqlite3_open("DATATEST.db", &db);
    };

    void closeConnection()
    {
        sqlite3_close_v2(db);
    }
};

