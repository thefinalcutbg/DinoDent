#include "SqliteBackend.h"
#include <sqlite3.h>

#include "GlobalSettings.h"
#include "View/ModalDialogBuilder.h"

constexpr const char* database_error_msg = 
    "Неуспешно записване в базата данни.\n"
    "Уверете се, че пътят към нея е правилен и\n"
    "че сте стартирали програмата като администратор.\n"
    ;

SqliteBackend::SqliteBackend()
    :

    stmt{nullptr}
{
    sqlite3_open(dbLocation.c_str(), &db_connection);

    if (db_connection == nullptr) {
         throw;
    }

    sqlite3_exec(db_connection, "PRAGMA foreign_keys = ON", NULL, NULL, NULL);
}

SqliteBackend::SqliteBackend(const std::string& query) : SqliteBackend()
{
    sqlite3_prepare_v2(db_connection, query.c_str(), -1, &stmt, NULL);
}

bool SqliteBackend::hasRows(){

    if(GlobalSettings::showDbDebugEnabled() && debug_hasRows){
        debug_hasRows = false;
        ModalDialogBuilder::showMultilineDialog(getPreparedStatement());
    }

    bool result = sqlite3_step(stmt) == SQLITE_ROW;//|| sqlite3_step(stmt) != ;

    if(!result){
        debug_hasRows = true;
        finalizeStatement();
    }

    return result;
}

int SqliteBackend::asInt(int column){ 
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_int(stmt, column); 
}

long long SqliteBackend::asRowId(int column)
{
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_int64(stmt, column);
}

long long SqliteBackend::asLongLong(int column)
{
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_int64(stmt, column);
}

bool SqliteBackend::asBool(int column)
{   
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return false;
    return static_cast<bool>(asInt(column));
}

double SqliteBackend::asDouble(int column)
{
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return 0;
    return sqlite3_column_double(stmt, column);
}

std::string SqliteBackend::asString(int column) { 
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return "";
    return reinterpret_cast<const char*>(sqlite3_column_text(stmt, column)); 
}

std::vector<unsigned char> SqliteBackend::asBlob(int column)
{
    std::vector<unsigned char> result;

    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) return result;

    auto ptr =  sqlite3_column_blob(stmt, column);

	result.assign(
		reinterpret_cast<const unsigned char*>(ptr),
		reinterpret_cast<const unsigned char*>(ptr) + sqlite3_column_bytes(stmt, column)
	);

    return result;
}


void SqliteBackend::newStatement(const std::string& query)
{ 
    finalizeStatement();
    sqlite3_prepare_v2(db_connection, query.c_str(), -1, &stmt, NULL);
}

bool SqliteBackend::execute(const std::string& query)
{
    char* err;
    finalizeStatement();

    int i = sqlite3_exec(db_connection, query.c_str(), NULL, NULL, &err);

    if (err){
        showDbError("Неуспешен запис в базата данни. Код на грешката: " + std::to_string(i));
    }

    finalizeStatement();

    return i == SQLITE_OK;
        
}

int SqliteBackend::columnCount() const
{
    if (stmt == nullptr) return 0;

    return sqlite3_column_count(stmt);
}

std::string SqliteBackend::columnName(int column) const
{
    if (stmt == nullptr) return std::string();

    return sqlite3_column_name(stmt, column);
}

int SqliteBackend::rowsAffected() const
{
    return sqlite3_changes(db_connection);
}

long long SqliteBackend::lastInsertedRowID()
{
    return sqlite3_last_insert_rowid(db_connection);
}

std::string SqliteBackend::getPreparedStatement() const
{
   char* ptr = sqlite3_expanded_sql(stmt);

   std::string result(ptr);

   sqlite3_free(ptr);

   return result;
}

void SqliteBackend::bind(int index, const std::string& value)
{
    if(stmt == nullptr) return;

    sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
}

void SqliteBackend::bind(int index, int value)
{
    if (stmt == nullptr) return;

    sqlite3_bind_int(stmt,index,value);
}

void SqliteBackend::bind(int index, double value)
{
    if (stmt == nullptr) return;

    sqlite3_bind_double(stmt, index, value);
}

void SqliteBackend::bind(int index, long long value)
{
    if (stmt == nullptr) return;

    sqlite3_bind_int64(stmt, index, value);
}

void SqliteBackend::bind(int index, const std::vector<unsigned char>& blob)
{
    if (stmt == nullptr) return;

    sqlite3_bind_blob(stmt, index, blob.data(), blob.size(), SQLITE_STATIC);
}

void SqliteBackend::bindNull(int index)
{
    if (stmt == nullptr) return;

    sqlite3_bind_null(stmt, index);
}

bool SqliteBackend::execute()
{
    if (stmt == nullptr) {

        showDbError("Невалидна заявка към базата данни");
        return false;
    }

    auto result = sqlite3_step(stmt);
    finalizeStatement();

    if (result != SQLITE_DONE) {
        showDbError("Неуспешен запис в базата данни. Код на грешката: " + std::to_string(result));
    }

    return result == SQLITE_DONE;
}


void SqliteBackend::finalizeStatement()
{
    if (!stmt) return;

    sqlite3_finalize(stmt);
    stmt = nullptr;
    
}

void SqliteBackend::setFilePath(const std::string& filePath)
{
    dbLocation = filePath;
}

int SqliteBackend::version()
{
    SqliteBackend db("PRAGMA user_version");

    while (db.hasRows()) {
        return db.asLongLong(0);
    }

    return -1;
}

void SqliteBackend::setVersion(int version)
{
    SqliteBackend::crudQuery("PRAGMA user_version =" + std::to_string(version));
}

bool SqliteBackend::crudQuery(const std::string& query)
{
    SqliteBackend db(query);

    return db.execute();
}


SqliteBackend::~SqliteBackend()
{
    finalizeStatement();

    if (db_connection) {
        int rc = sqlite3_close_v2(db_connection);

        db_connection = nullptr;
    }

}


void SqliteBackend::showDbError(const std::string& msg)
{
    if (!s_showErrorDialog) return;
    
    error_count++;

	if (error_count > 5) return; //prevent spamming the user with error dialogs

    ModalDialogBuilder::showError(msg);
}

#include <QFileInfo>
#include <QDir>

bool SqliteBackend::createDirPath()
{
    QFileInfo info(QString::fromStdString(dbLocation));

    QDir dir = info.absoluteDir();
    if (dir.isEmpty() || !dir.mkpath(".")) return false;

    return true;

}
