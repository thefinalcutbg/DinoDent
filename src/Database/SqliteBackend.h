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

    void finalizeStatement();

	void showDbError(const std::string& msg);

public:
   
    //open new connection and execute query on the go
    static bool crudQuery(const std::string& query); 
    static bool createDirPath(const std::string& filePath);
    static void setShowErrors(bool show) { s_showErrorDialog = show;}


    SqliteBackend(const std::string& filePath);

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

    bool backup() override;

    virtual ~SqliteBackend();

};

