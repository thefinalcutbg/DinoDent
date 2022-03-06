#include "Database.h"
#include "QDebug"
Database::Database() : err(nullptr), db(nullptr), stmt(nullptr)
{

    sqlite3_open("DATATEST.db", &db);

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
        "CREATE TABLE patient (type INT NOT NULL, id VARCHAR (10) NOT NULL PRIMARY KEY, birth VARCHAR (10) NOT NULL, sex BOOLEAN NOT NULL, fname VARCHAR (50) NOT NULL, mname VARCHAR (50), lname VARCHAR (50) NOT NULL, city VARCHAR (100) NOT NULL, address VARCHAR (100), hirbno VARCHAR (8), phone VARCHAR (20), allergies VARCHAR (400), currentDiseases VARCHAR (400), pastDiseases VARCHAR (400))"
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


    if (rc != SQLITE_OK)
    {
        //qDebug() << "Error opening DB:" << QString::fromStdString(sqlite3_errmsg(db));
    }

    sqlite3_close(db);
}