#include "Database.h"
#include "View/ModalDialogBuilder.h"
#include <sqLite3/sqlite3.h>
#include "Resources.h"
#include <filesystem>

Db::Db(Db* existingConnection)
    :
    m_connectionOwned{ existingConnection == nullptr },
    stmt{nullptr}
{
    if(m_connectionOwned){
        int i = sqlite3_open(dbLocation.c_str(), &db_connection);
    }
    else
    {
        db_connection = existingConnection->db_connection;
    }
    
    if (db_connection == nullptr) {
         throw;
    }

  //  execute("PRAGMA foreign_keys = ON");
    
}

Db::Db(const std::string& query, Db* existingConnection) : Db(existingConnection)
{
    sqlite3_prepare_v2(db_connection, query.c_str(), -1, &stmt, NULL);
}

bool Db::hasRows(){

    return sqlite3_step(stmt) == SQLITE_ROW;//|| sqlite3_step(stmt) != ;
}

int Db::asInt(int column){ 
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_int(stmt, column); 
}

long long Db::asRowId(int column)
{
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_int64(stmt, column);
}

long long Db::asLongLong(int column)
{
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_int64(stmt, column);
}

bool Db::asBool(int column)
{   
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return false;
    return static_cast<bool>(asInt(column));
}

double Db::asDouble(int column)
{
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_double(stmt, column);
}

std::string Db::asString(int column) { 
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return "";
    return reinterpret_cast<const char*>(sqlite3_column_text(stmt, column)); 
}


void Db::newStatement(const std::string& query)
{
    if (stmt != nullptr) {
        sqlite3_finalize(stmt);
    }
 
    sqlite3_prepare_v2(db_connection, query.c_str(), -1, &stmt, NULL);
}

bool Db::execute(const std::string& query)
{
    if (stmt != nullptr) {
        sqlite3_finalize(stmt);
    }

    char* err;

    int i = sqlite3_exec(db_connection, query.c_str(), NULL, NULL, &err);

    if (err && s_showError) {
        ModalDialogBuilder::showMessage(u8"Неуспешно записване в базата данни");
        ModalDialogBuilder::showMultilineDialog(query);
    }

    return i == SQLITE_OK;
        
}

bool Db::execute()
{
    if (stmt == nullptr) return false;

    if (sqlite3_step(stmt) != SQLITE_DONE) return false;

    return true;
}

long long Db::lastInsertedRowID()
{
    return sqlite3_last_insert_rowid(db_connection);
}

void Db::closeConnection()
{
    if (db_connection) sqlite3_close_v2(db_connection);
}

bool Db::bind(int index, const std::string& value)
{
    if(stmt == nullptr) return false;

    if (sqlite3_bind_text(
        stmt,
        index,
        value.c_str(),
        value.size(),  // length of text
        SQLITE_STATIC
    ) != SQLITE_OK) {
        return false;
    }

    return true;
}

bool Db::bind(int index, int value)
{
    if (stmt == nullptr) return false;

    if (sqlite3_bind_int(
        stmt,
        index,  // Index of wildcard
        value
    )
        != SQLITE_OK) {
        return false;
    }

    return true;
}

bool Db::bind(int index, double value)
{
    if (stmt == nullptr) return false;

    if (sqlite3_bind_double(
        stmt,
        index,  // Index of wildcard
        value
    )
        != SQLITE_OK) {
        return false;
    }

    return true;
}

bool Db::bind(int index, long long value)
{
    if (stmt == nullptr) return false;

    if (sqlite3_bind_int64(
        stmt,
        index,  // Index of wildcard
        value
    )
        != SQLITE_OK) {
        return false;
    }

    return true;


}

void Db::setFilePath(const std::string& filePath)
{
    dbLocation = filePath;
}

int Db::version()
{
    Db db("PRAGMA user_version");

    while (db.hasRows()) {
        return db.asLongLong(0);
    }

    return -1;
}

void Db::setVersion(int version)
{
    Db::crudQuery("PRAGMA user_version =" + std::to_string(version));
}

bool Db::crudQuery(const std::string& query)
{
    char* err;
    sqlite3* db;

    int i = sqlite3_open(dbLocation.c_str(), &db);
    if (db == nullptr) {
        return false;
    }

   // sqlite3_exec(db, "PRAGMA foreign_keys = ON", NULL, NULL, &err);

    i = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    if (err && s_showError) {
        ModalDialogBuilder::showMessage(u8"Неуспешна операция в базата данни");
        ModalDialogBuilder::showMultilineDialog(query);
    }

    return i == SQLITE_OK;
}


Db::~Db()
{
    if (stmt != nullptr) {
        sqlite3_finalize(stmt);
    }

    if (m_connectionOwned && db_connection) {
        sqlite3_close_v2(db_connection);
    }
}


void Db::createIfNotExist()
{
    if (std::filesystem::exists("TorqueDB.db")) return;

    Db db;

    for (auto& tableSchema : Resources::dbSchema())
    {
        db.execute(tableSchema);
    }

  //  rc = sqlite3_exec(db,"VACUUM", NULL, NULL, &err);
    s_showError = true;
}