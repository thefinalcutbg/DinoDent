#pragma once
#include "DbBackend.h"

struct sqlite3;
struct sqlite3_stmt;

class SqliteBackend final : public DbBackend
{
	int error_count{ 0 };

    sqlite3* db_connection;

    sqlite3_stmt* stmt;

    bool debug_hasRows = true;

    static inline bool s_showErrorDialog{ false };
    static inline std::string dbLocation{ "database.db" };

    void finalizeStatement();

	void showDbError(const std::string& msg);

public:
    static void setFilePath(const std::string& filePath);
    static std::string& getFilePath() { return dbLocation; }
    static int version();
    static void setVersion(int version);
    //open new connection and execute query on the go
    static bool crudQuery(const std::string& query); 
    static bool createDirPath();
    static void setShowErrors(bool show) { s_showErrorDialog = show;}


    SqliteBackend();

    SqliteBackend(const std::string& query);
     
    //returns true if there are more rows to get from database
    bool hasRows(); 
    int asInt(int column);
    long long asRowId(int column);
    long long asLongLong(int column);
    bool asBool(int column);
    double asDouble(int column);
    std::string asString(int column);
    std::vector<unsigned char> asBlob(int column);
    void newStatement(const std::string& query);
    bool execute(const std::string& query);
    int columnCount() const;
    std::string columnName(int column) const;
    int rowsAffected() const;
    long long lastInsertedRowID();
    std::string getPreparedStatement() const;


    //bindings with prepared statement:
    void bind(int index, const std::string& value);
    void bind(int index, int value);
    void bind(int index, double value);
    void bind(int index, long long value);
    void bind(int index, const std::vector<unsigned char>& blob);
    void bindNull(int index);
    //executes already prepared statement with bindings
    bool execute();

    virtual ~SqliteBackend();

};

