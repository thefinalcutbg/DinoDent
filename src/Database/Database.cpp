#include "Database.h"

#include <sqLite/sqlite3.h>

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

   // execute("PRAGMA foreign_keys = ON");
    
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

long long Db::asLongLong(int column)
{
    return sqlite3_column_int64(stmt, column);
}

bool Db::asBool(int column)
{
    return static_cast<bool>(asInt(column));
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
#include <QDebug>
bool Db::execute(const std::string& query)
{
    if (stmt != nullptr) {
        sqlite3_finalize(stmt);
    }

    char* err;

    int i = sqlite3_exec(db_connection, query.c_str(), NULL, NULL, &err);

    if (err) {
        qDebug() << QString::fromStdString(query);
        qDebug() << err;
    }

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

   // sqlite3_exec(db, "PRAGMA foreign_keys = ON", NULL, NULL, &err);

    i = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    if (err) {
        qDebug() << QString::fromStdString(query);
        qDebug() << err;
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

    Db db;

    db.execute(
        "CREATE TABLE practice (rzi VARCHAR NOT NULL PRIMARY KEY, name VARCHAR NOT NULL, bulstat VARCHAR NOT NULL, firm_address VARCHAR, practice_address VARCHAR, pass VARCHAR NOT NULL, legal_entity INT, vat VARCHAR, nzok_contract VARCHAR, priceList VARCHAR, settings VARCHAR)"
    );

    db.execute(
        "CREATE TABLE doctor (lpk VARCHAR (9) NOT NULL PRIMARY KEY, pass VARCHAR NOT NULL, fname VARCHAR (50) NOT NULL, mname VARCHAR NOT NULL, lname VARCHAR (50) NOT NULL, spec INT NOT NULL, egn VARCHAR (10) NOT NULL, several_rhif INTEGER)"
    );

    db.execute(
        "CREATE TABLE practice_doctor (practice_rzi VARCHAR NOT NULL REFERENCES practice ON DELETE CASCADE ON UPDATE CASCADE, doctor_lpk VARCHAR NOT NULL, admin INT NOT NULL, FOREIGN KEY (doctor_lpk) REFERENCES doctor (lpk) ON DELETE CASCADE ON UPDATE CASCADE)"
    );

    db.execute(
        "CREATE TABLE patient (rowid INTEGER PRIMARY KEY, type INT NOT NULL, id VARCHAR (10) NOT NULL, birth VARCHAR (10) NOT NULL, sex BOOLEAN NOT NULL, fname VARCHAR (50) NOT NULL, mname VARCHAR (50), lname VARCHAR (50) NOT NULL, city INT NOT NULL, address VARCHAR (100), hirbno VARCHAR (8), phone VARCHAR (20), allergies VARCHAR (400), currentDiseases VARCHAR (400), pastDiseases VARCHAR (400))"
    );

    db.execute(
        "CREATE TABLE amblist (rowid INTEGER NOT NULL PRIMARY KEY, patient_rowid INTEGER NOT NULL, day INT NOT NULL, month INT NOT NULL, year INT NOT NULL, num INT NOT NULL, lpk VARCHAR (9) NOT NULL REFERENCES doctor (lpk) ON DELETE CASCADE ON UPDATE CASCADE, rzi VARCHAR (10) REFERENCES practice (rzi) ON DELETE CASCADE ON UPDATE CASCADE NOT NULL, fullCoverage INT NOT NULL, charge INT NOT NULL, status_json VARCHAR, FOREIGN KEY (patient_rowid) REFERENCES patient (rowid) ON DELETE CASCADE ON UPDATE CASCADE)"
    );

    db.execute(
        "CREATE TABLE procedure (rowid INTEGER NOT NULL PRIMARY KEY, amblist_rowid INTEGER NOT NULL, nzok INT NOT NULL, code VARCHAR (10) NOT NULL, ksmp VARCHAR (8) NOT NULL, type INT NOT NULL, day INT NOT NULL, tooth INT NOT NULL, deciduous INT NOT NULL, price REAL NOT NULL, data VARCHAR NOT NULL, FOREIGN KEY (amblist_rowid) REFERENCES amblist (rowid) ON DELETE CASCADE ON UPDATE CASCADE)"
    );

    db.execute(
        "CREATE TABLE periostatus (rowid INTEGER NOT NULL PRIMARY KEY, patient_rowid INTEGER NOT NULL REFERENCES patient (rowid) ON DELETE CASCADE ON UPDATE CASCADE, lpk VARCHAR (9) NOT NULL REFERENCES doctor (lpk) ON DELETE CASCADE ON UPDATE CASCADE, rzi VARCHAR (10) NOT NULL REFERENCES practice (rzi) ON DELETE CASCADE ON UPDATE CASCADE, year INT NOT NULL, month INT NOT NULL, day INT NOT NULL, data VARCHAR NOT NULL, FOREIGN KEY (patient_rowid) REFERENCES patient (rowid) ON DELETE CASCADE ON UPDATE CASCADE)"
    );

    db.execute(
        "CREATE TABLE note (rowid INTEGER, patient_rowid INTEGER NOT NULL, tooth INT NOT NULL, text VARCHAR NOT NULL, PRIMARY KEY (patient_rowid, tooth), FOREIGN KEY (patient_rowid) REFERENCES patient (rowid) ON DELETE CASCADE ON UPDATE CASCADE)"
    );

    db.execute(
        "CREATE TABLE financial (rowid INTEGER PRIMARY KEY, practice_rzi VARCHAR (10) NOT NULL REFERENCES practice (rzi) ON DELETE CASCADE ON UPDATE CASCADE, num BIGINT NOT NULL, type INT NOT NULL, day INT NOT NULL, month INT NOT NULL, year INT NOT NULL, month_notif INT, recipient_id VARCHAR (100), recipient_name, recipient_phone VARCHAR (20), recipient_address, data TEXT NOT NULL)"
    );

  //  rc = sqlite3_exec(db,"VACUUM", NULL, NULL, &err);


}