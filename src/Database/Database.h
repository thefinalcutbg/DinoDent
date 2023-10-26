#pragma once

#include <string>

struct sqlite3;
struct sqlite3_stmt;

class Db
{
    sqlite3* db_connection;
    bool m_connectionOwned;

    int total_bindings{ 0 };
    int successful_bindings{ 0 };

    sqlite3_stmt* stmt;

    static inline bool s_showError{ false };
    static inline std::string dbLocation{ "database.db" };

    void finalizeStatement();


public:
    static void setFilePath(const std::string& filePath);
    static std::string& getFilePath() { return dbLocation; }
    static int version();
    static void setVersion(int version);
    //open new connection and execute query on the go
    static bool crudQuery(const std::string& query); 
    static bool createIfNotExist();
    static void showErrorDialog(bool show) {s_showError = show;}

    //If connection exists, db finalizes statement in destructor, but does not break connection
    Db(Db* existingConnection = nullptr);
    //Create db connection with statement ready for retrieval:
    Db(const std::string& query, Db* existingConnection = nullptr);
     
    //returns true if there are more rows to get from database
    bool hasRows(); 
    int asInt(int column);
    long long asRowId(int column);
    long long asLongLong(int column);
    bool asBool(int column);
    double asDouble(int column);
    std::string asString(int column);
    void newStatement(const std::string& query);
    bool execute(const std::string& query);

    long long lastInsertedRowID();
    std::string getPreparedStatement();
    void closeConnection();


    //bindings with prepared statement:
    void bind(int index, const std::string& value);
    void bind(int index, int value);
    void bind(int index, double value);
    void bind(int index, long long value);
    void bindNull(int index);
    //executes already prepared statement with bindings
    bool execute();

    ~Db();

};

