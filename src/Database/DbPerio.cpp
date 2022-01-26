#include "DbPerio.h"
#include "Model/Parser/Parser.h"
#include "Model/Procedure/Procedure.h"
#include "QDebug"

ToothContainer DbPerio::getStatus(const std::string& patientID, const Date& date)
{
	openConnection();

    std::string jsonStatus;
    std::string amblistId;
    std::string LPK;

    std::string query = "SELECT status_json, id, LPK FROM amblist WHERE "
        " patient_id = '" + patientID + "'"
        " AND year <= " + std::to_string(date.year) +
        " AND month <= " + std::to_string(date.month) +
        " ORDER BY id DESC LIMIT 1";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        jsonStatus = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        amblistId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        LPK = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    }

    sqlite3_finalize(stmt);

    if (jsonStatus.empty())
    {
        closeConnection();
        return ToothContainer();
    }
    
    ToothContainer toothStatus;

    Parser::parse(jsonStatus, toothStatus);

    query = "SELECT type, tooth, temp, data FROM procedure WHERE"
            " amblist_id = '" + amblistId + "'"
            " AND day <= " + std::to_string(date.day) +
            " ORDER BY seq";



    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);


    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        Procedure p;
        p.LPK = LPK;
        p.type = static_cast<ProcedureType>(sqlite3_column_int(stmt, 0));
        p.tooth = sqlite3_column_int(stmt, 1);
        p.temp = sqlite3_column_int(stmt, 2);
        Parser::parse(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))), p);
        p.applyProcedure(toothStatus);
    }

    closeConnection();

    return toothStatus;
   
}

PerioStatus DbPerio::getPerioStatus(const std::string& patientID, Date date)
{
    openConnection();

    PerioStatus perioStatus;

    std::string query = "SELECT id, day, month, year, data FROM periostatus WHERE"
        " patient_id = '" + patientID + "'"
        " AND year <= " + std::to_string(date.year) +
        " AND month <= " + std::to_string(date.month) +
        " AND day <= " + std::to_string(date.day) +
        " ORDER BY id DESC LIMIT 1";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {

        perioStatus.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        perioStatus.date = Date(
                                        sqlite3_column_int(stmt, 1),
                                        sqlite3_column_int(stmt, 2),
                                        sqlite3_column_int(stmt, 3)
                                );

        Parser::parse(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))), perioStatus);
    }

    closeConnection();

    return perioStatus;
        
}

PerioStatus DbPerio::getPerioStatus(const std::string& perioID)
{

    openConnection();

    PerioStatus perioStatus;

    std::string query = "SELECT id, day, month, year, data FROM periostatus WHERE"
        " id = '" + perioID + "'";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {

        perioStatus.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        perioStatus.date = Date(
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3)
        );

        Parser::parse(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))), perioStatus);
    }

    closeConnection();

    return perioStatus;

}

void DbPerio::insertPerioStatus(PerioStatus& perioStatus, const std::string& patientID)
{
    openConnection();

    std::string query =
        "INSERT INTO periostatus (day, month, year, patient_id, data) VALUES ('"
        + std::to_string(perioStatus.date.day) + "','"
        + std::to_string(perioStatus.date.month) + "','"
        + std::to_string(perioStatus.date.year) + "','"
        + patientID + "','"
        + Parser::write(perioStatus) + "')";

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    perioStatus.id = std::to_string((int)sqlite3_last_insert_rowid(db));

    if (rc != SQLITE_OK) qDebug() << "Update error:";// << &db;

    closeConnection();
}

void DbPerio::updatePerioStatus(const PerioStatus& perioStatus)
{
    openConnection();

    std::string query = "UPDATE periostatus SET"
        " day = " + std::to_string(perioStatus.date.day) +
        ", month = " + std::to_string(perioStatus.date.month) +
        ", year = " + std::to_string(perioStatus.date.year) +
        ", data = '" + Parser::write(perioStatus) + "' "
        "WHERE id = " + perioStatus.id;

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    if (rc != SQLITE_OK) qDebug() << "Update error:";// << &db;

    closeConnection();

}


