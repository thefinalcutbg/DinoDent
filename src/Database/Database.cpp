#include "database.h"
#include <qdebug.h>

Database::Database() : db(NULL), err(NULL), stmt(NULL)
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
        "pastDiseases       VARCHAR(400)"
        ");"

        "CREATE TABLE IF NOT EXISTS amblist("
        "id              INTEGER         NOT NULL   PRIMARY KEY,"
        "day             INT             NOT NULL,"
        "month           INT             NOT NULL,"
        "year            INT             NOT NULL,"
        "num             INT             NOT NULL,"
        "patient_id      VARCHAR(10)     NOT NULL,"
        "lpk             VARCHAR(9)      NOT NULL,"
        "test            VARCHAR,"
        "status_json     BLOB," 
        "FOREIGN KEY    (patient_id) REFERENCES patient(id)"
        ");"

        , NULL, NULL, &err);
    
    if (rc != SQLITE_OK)
    {
        std::cout << "Error opening DB:" << sqlite3_errmsg(db);
        sqlite3_close(db);
    }

}



