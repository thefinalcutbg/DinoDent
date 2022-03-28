#pragma once

#include <string>

struct sqlite3;
struct sqlite3_stmt;

class Db
{
    sqlite3* db_connection;
    bool m_connectionOwned;
    sqlite3_stmt* stmt;


    static inline std::string dbLocation{ "DATATEST.db" };


public:
    static void setFilePath(const std::string& filePath);

    //open new connection and execute query on the go
    static bool crudQuery(const std::string& query); 
    static void createIfNotExist();

    //If connection exists, db finalizes statement in destructor, but does not break connection
    Db(Db* existingConnection = nullptr);
    //Create db connection with statement ready for retrieval:
    Db(const std::string& query, Db* existingConnection = nullptr);

    //returns true if there are more rows to get from database
    bool hasRows(); 
    int asInt(int column);
    long long asRowId(int column);
    double asDouble(int column);
    std::string asString(int column);
    void newStatement(const std::string& query);
    bool execute(const std::string& query);
    long long lastInsertedRowID();
    void closeConnection();
    ~Db();

};

