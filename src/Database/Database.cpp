#include "Database.h"

Database::Database() : err(nullptr), db(nullptr), stmt(nullptr)
{

    sqlite3_open("DATATEST.db", &db);

    rc = sqlite3_exec(
        db,
        "CREATE TABLE IF NOT EXISTS patient("
        "type               INT             NOT NULL,"
        "id                 VARCHAR(10)     NOT NULL    PRIMARY KEY, "
        "birth              VARCHAR(10)     NOT NULL,"
        "sex                BOOLEAN         NOT NULL,"
        "fname              VARCHAR(50)     NOT NULL,"
        "mname              VARCHAR(50),"
        "lname              VARCHAR(50)     NOT NULL,"
        "city               VARCHAR(100)    NOT NULL,"
        "address            VARCHAR(100),"
        "hirbno             VARCHAR(8),"
        "phone              VARCHAR(20),"
        "allergies          VARCHAR(400)," 
        "currentDiseases    VARCHAR(400)," 
        "pastDiseases       VARCHAR(400) "  
        ")", NULL, NULL, &err);

    rc = sqlite3_exec(db,
        "CREATE TABLE IF NOT EXISTS amblist("
        "id              INTEGER         NOT NULL   PRIMARY KEY,"
        "day             INT             NOT NULL,"
        "month           INT             NOT NULL,"
        "year            INT             NOT NULL,"
        "num             INT             NOT NULL,"
        "patient_id      VARCHAR(10)     NOT NULL,"
        "lpk             VARCHAR(9)      NOT NULL,"
        "unfavourable    INT             NOT NULL,"
        "charge          INT             NOT NULL,"
        "status_json     VARCHAR,"
        "FOREIGN KEY    (patient_id) REFERENCES patient(id) ON DELETE CASCADE ON UPDATE CASCADE"
        ")"
        , NULL, NULL, &err);
 
    rc = sqlite3_exec(
        db,
        "CREATE TABLE IF NOT EXISTS procedure("
        "id              INTEGER         NOT NULL PRIMARY KEY,"
        "nzok            INT             NOT NULL,"   
        "code            VARCHAR(10)     NOT NULL,"
        "seq             INT             NOT NULL,"  //the sequence of the manipulation(if dates are the same)
        "type            INT             NOT NULL," //required for json parser and statistics
        "day             INT             NOT NULL,"
        "tooth           INT             NOT NULL,"
        "temp            INT             NOT NULL,"
        "price           REAL            NOT NULL,"
        "data            VARCHAR         NOT NULL," //json data depending on type
        "amblist_id      INT             NOT NULL,"
        "FOREIGN KEY    (amblist_id)     REFERENCES amblist(id) ON DELETE CASCADE ON UPDATE CASCADE"
        ")"
        , NULL, NULL, &err);

    rc = sqlite3_exec(
        db,
        "CREATE TABLE IF NOT EXISTS note("
        "patient_id      VARCHAR(10)     NOT NULL,"
        "tooth           INT             NOT NULL,"
        "text            VARCHAR         NOT NULL,"
        "PRIMARY KEY    (patient_id, tooth), "
        "FOREIGN KEY    (patient_id)     REFERENCES patient(id) ON DELETE CASCADE ON UPDATE CASCADE"
        ")"
        , NULL, NULL, &err);

    if (rc != SQLITE_OK)
    {
        //qDebug() << "Error opening DB:" << QString::fromStdString(sqlite3_errmsg(db));
    }

    sqlite3_close(db);
}



