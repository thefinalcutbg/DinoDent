#include "DbPerio.h"
#include "Model/Parser/Parser.h"
#include "Model/Procedure/Procedure.h"
#include "Database.h"

ToothContainer DbPerio::getStatus(const std::string& patientID, const Date& date)
{


    std::string jsonStatus;
    std::string amblistId;
    std::string LPK;

    Db db(

        "SELECT status_json, id, LPK FROM amblist WHERE "
        " patient_id = '" + patientID + "'"
        " AND year <= " + std::to_string(date.year) +
        " AND month <= " + std::to_string(date.month) +
        " ORDER BY id DESC LIMIT 1"
    );

    while (db.hasRows())
    {
        jsonStatus = db.asString(0);
        amblistId = db.asString(1);
        LPK = db.asString(2);
    }

    if (jsonStatus.empty())
    {
        return ToothContainer();
    }
    
    ToothContainer toothStatus;

    Parser::parse(jsonStatus, toothStatus);


    db.newStatement(
    
        "SELECT type, tooth, temp, data FROM procedure WHERE"
        " amblist_id = '" + amblistId + "'"
        " AND day <= " + std::to_string(date.day) +
        " ORDER BY seq"

    );


    while (db.hasRows())
    {
        Procedure p;
        p.LPK = LPK;
        p.type = static_cast<ProcedureType>(db.asInt(0));
        p.tooth = db.asInt(1);
        p.temp = db.asInt(2);
        Parser::parse(db.asString(3), p);
        p.applyProcedure(toothStatus);
    }

    return toothStatus;
   
}

PerioStatus DbPerio::getPerioStatus(const std::string& patientID, Date date)
{
    PerioStatus perioStatus;

    std::string query = "SELECT id, day, month, year, data FROM periostatus WHERE"
        " patient_id = '" + patientID + "'"
        " AND year <= " + std::to_string(date.year) +
        " AND month <= " + std::to_string(date.month) +
        " AND day <= " + std::to_string(date.day) +
        " ORDER BY id DESC LIMIT 1";

    for(Db db(query); db.hasRows();)
    {

        perioStatus.id = db.asString(0);
        perioStatus.date = Date(
                                        db.asInt(1),
                                        db.asInt(2),
                                        db.asInt(3)
                                );

        Parser::parse(db.asString(4), perioStatus);
    }

    return perioStatus;
        
}

PerioStatus DbPerio::getPerioStatus(const std::string& perioID)
{
    PerioStatus perioStatus;

    Db db(
        "SELECT id, day, month, year, data FROM periostatus WHERE"
        " id = '" + perioID + "'");

    while (db.hasRows())
    {

        perioStatus.id = db.asString(0);
        perioStatus.date = Date(
            db.asInt(1),
            db.asInt(2),
            db.asInt(3)
        );

        Parser::parse(db.asString(4), perioStatus);
    }

    return perioStatus;
}

void DbPerio::insertPerioStatus(PerioStatus& perioStatus, const std::string& patientID)
{

    std::string query =
        "INSERT INTO periostatus (day, month, year, patient_id, data) VALUES ('"
        + std::to_string(perioStatus.date.day) + "','"
        + std::to_string(perioStatus.date.month) + "','"
        + std::to_string(perioStatus.date.year) + "','"
        + patientID + "','"
        + Parser::write(perioStatus) + "')";

    Db db;
    db.execute(query);

    perioStatus.id = db.lastInsertedRowID();
}

void DbPerio::updatePerioStatus(const PerioStatus& perioStatus)
{
    Db::crudQuery(

        "UPDATE periostatus SET"
        " day = " + std::to_string(perioStatus.date.day) +
        ", month = " + std::to_string(perioStatus.date.month) +
        ", year = " + std::to_string(perioStatus.date.year) +
        ", data = '" + Parser::write(perioStatus) + "' "
        "WHERE id = " + perioStatus.id

    );

}


