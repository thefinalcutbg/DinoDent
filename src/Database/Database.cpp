#include "Database.h"

#include "Libraries/sqLite/sqlite3.h"

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
    
}

Db::Db(const std::string& query, Db* existingConnection) : Db(existingConnection)
{
    sqlite3_prepare_v2(db_connection, query.c_str(), -1, &stmt, NULL);
}


bool Db::hasRows(){
    return sqlite3_step(stmt) != SQLITE_DONE;
}

int Db::asInt(int column){ 
    return sqlite3_column_int(stmt, column); 
}

long long Db::asRowId(int column)
{
    return sqlite3_column_int64(stmt, column);
}

double Db::asDouble(int column)
{
    return sqlite3_column_double(stmt, column);
}

std::string Db::asString(int column) { 
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
    sqlite3_finalize(stmt);

    char* err;

    int i = sqlite3_exec(db_connection, query.c_str(), NULL, NULL, &err);
    return i == SQLITE_OK;
        
}

long long Db::lastInsertedRowID()
{
    return sqlite3_last_insert_rowid(db_connection);
}

void Db::closeConnection()
{
    if (db_connection) sqlite3_close_v2(db_connection);
}

void Db::setFilePath(const std::string& filePath)
{
    dbLocation = filePath;
}

bool Db::crudQuery(const std::string& query)
{
    char* err;
    sqlite3* db;

    int i = sqlite3_open(dbLocation.c_str(), &db);
    if (db == nullptr) {
        return false;
    }

    i = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (i != SQLITE_OK) {
        return false;
    }

    return true;
}


Db::~Db()
{
    sqlite3_finalize(stmt);

    if (m_connectionOwned && db_connection) {
        sqlite3_close_v2(db_connection);
    }
}



void Db::createIfNotExist()
{
    sqlite3* db;

    sqlite3_open(dbLocation.c_str(), &db);
    char* err;
    int rc;

    rc = sqlite3_exec(db, "PRAGMA foreign_keys = ON", NULL, NULL, &err);

    rc = sqlite3_exec(
        db,
        "CREATE TABLE note(patient_id VARCHAR(10) NOT NULL, tooth INT NOT NULL,text VARCHAR NOT NULL,PRIMARY KEY (patient_id, tooth), FOREIGN KEY (patient_id) REFERENCES patient(id) ON DELETE CASCADE ON UPDATE CASCADE)"
        , NULL, NULL, &err);

    rc = sqlite3_exec(db,
        "CREATE TABLE procedure (id INTEGER NOT NULL PRIMARY KEY, nzok INT NOT NULL, code VARCHAR (10) NOT NULL, seq INT NOT NULL, type INT NOT NULL, day INT NOT NULL, tooth INT NOT NULL, \"temp\" INT, price REAL NOT NULL, data VARCHAR NOT NULL, amblist_id INT NOT NULL, FOREIGN KEY (amblist_id) REFERENCES amblist ON DELETE CASCADE ON UPDATE CASCADE)"
        , NULL, NULL, &err);

    rc = sqlite3_exec(
        db,
        "CREATE TABLE patient (type INT NOT NULL, id VARCHAR (10) NOT NULL PRIMARY KEY, birth VARCHAR (10) NOT NULL, sex BOOLEAN NOT NULL, fname VARCHAR (50) NOT NULL, mname VARCHAR (50), lname VARCHAR (50) NOT NULL, city INT NOT NULL, address VARCHAR (150), hirbno VARCHAR (8), phone VARCHAR (20), allergies VARCHAR (400), currentDiseases VARCHAR (400), pastDiseases VARCHAR (400))"
        , NULL, NULL, &err);

    rc = sqlite3_exec(
        db,
        "CREATE TABLE periostatus (id INTEGER NOT NULL PRIMARY KEY, patient_id VARCHAR (10) NOT NULL, year INT NOT NULL, month INT NOT NULL, day INT NOT NULL, data VARCHAR NOT NULL, FOREIGN KEY (patient_id) REFERENCES patient (id) ON DELETE CASCADE ON UPDATE CASCADE)"
        , NULL, NULL, &err);

    rc = sqlite3_exec(
        db,
        "CREATE TABLE amblist (id INTEGER NOT NULL PRIMARY KEY, day INT NOT NULL, month INT NOT NULL, year INT NOT NULL, num INT NOT NULL, patient_id VARCHAR (10) NOT NULL, lpk VARCHAR (9) NOT NULL REFERENCES doctor (lpk) ON DELETE CASCADE ON UPDATE CASCADE, rzi VARCHAR (10) REFERENCES practice (rzi) ON DELETE CASCADE ON UPDATE CASCADE NOT NULL, fullCoverage INT NOT NULL, charge INT NOT NULL, status_json VARCHAR)"
        , NULL, NULL, &err);

    rc = sqlite3_exec(
        db,
        "CREATE TABLE practice (rzi VARCHAR NOT NULL PRIMARY KEY, name VARCHAR NOT NULL, bulstat VARCHAR NOT NULL, firm_address VARCHAR, practice_address VARCHAR, pass VARCHAR NOT NULL, legal_entity INT, vat VARCHAR, nzok_contract VARCHAR, priceList VARCHAR)"
        , NULL, NULL, &err);

    rc = sqlite3_exec(
        db,
        "CREATE TABLE doctor (lpk VARCHAR (9) NOT NULL PRIMARY KEY, pass VARCHAR NOT NULL, fname VARCHAR (50) NOT NULL, mname VARCHAR NOT NULL, lname VARCHAR (50) NOT NULL, spec INT NOT NULL, egn VARCHAR (10) NOT NULL, several_rhif INTEGER)"
        , NULL, NULL, &err);

    rc = sqlite3_exec(
        db,
        "CREATE TABLE practice_doctor (practice_rzi VARCHAR (10) NOT NULL REFERENCES practice ON DELETE CASCADE ON UPDATE CASCADE, doctor_lpk VARCHAR (9) NOT NULL, admin INT NOT NULL, FOREIGN KEY (doctor_lpk) REFERENCES doctor (lpk) ON DELETE CASCADE ON UPDATE CASCADE)"
        , NULL, NULL, &err);

  //  rc = sqlite3_exec(db,"VACUUM", NULL, NULL, &err);

    if (rc != SQLITE_OK)
    {
        //qDebug() << "Error opening DB:" << QString::fromStdString(sqlite3_errmsg(db));
    }

    sqlite3_close(db);
}