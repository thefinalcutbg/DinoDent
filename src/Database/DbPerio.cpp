#include "DbPerio.h"
#include "Model/Parser/Parser.h"
#include "Model/Procedure/Procedure.h"
#include "Database.h"

ToothContainer DbPerio::getStatus(long long patientRowId, const Date& date)
{


    std::string jsonStatus;
    long long amblistId{0};
    std::string LPK;

    Db db(

        "SELECT status_json, rowid, LPK FROM amblist WHERE "
        " patient_rowid = " + std::to_string(patientRowId) +
        " AND year <= " + std::to_string(date.year) +
        " AND month <= " + std::to_string(date.month) +
        " ORDER BY rowid DESC LIMIT 1"
    );

    while (db.hasRows())
    {
        jsonStatus = db.asString(0);
        amblistId = db.asRowId(1);
        LPK = db.asString(2);
    }

    if (jsonStatus.empty())
    {
        return ToothContainer();
    }
    
    ToothContainer toothStatus;

    Parser::parse(jsonStatus, toothStatus);


    db.newStatement(
    
        "SELECT type, tooth, deciduous, data FROM procedure WHERE"
        " amblist_rowid = " + std::to_string(amblistId) +
        " AND day <= " + std::to_string(date.day) +
        " ORDER BY rowid"

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

PerioStatus DbPerio::getPerioStatus(long long patientRowId, Date date)
{
    PerioStatus perioStatus;

    std::string query = "SELECT rowid, lpk, day, month, year, data FROM periostatus WHERE"
        " patient_rowid = " + std::to_string(patientRowId) +
        " AND year <= " + std::to_string(date.year) +
        " AND month <= " + std::to_string(date.month) +
        " AND day <= " + std::to_string(date.day) +
        " ORDER BY rowid DESC LIMIT 1";

    for(Db db(query); db.hasRows();)
    {

        perioStatus.rowid = db.asRowId(0);
        perioStatus.LPK = db.asString(1);
        perioStatus.date = Date(
                                        db.asInt(2),
                                        db.asInt(3),
                                        db.asInt(4)
                                );

        Parser::parse(db.asString(5), perioStatus);
    }

    return perioStatus;
        
}

PerioStatus DbPerio::getPerioStatus(long long rowid)
{
    PerioStatus perioStatus;

    Db db(
        "SELECT rowid, lpk, day, month, year, data FROM periostatus WHERE"
        " rowid = " + std::to_string(rowid));

    while (db.hasRows())
    {

        perioStatus.rowid = db.asRowId(0);
        perioStatus.LPK = db.asString(1);
        perioStatus.date = Date(
            db.asInt(2),
            db.asInt(3),
            db.asInt(4)
        );

        Parser::parse(db.asString(5), perioStatus);
    }

    return perioStatus;
}

std::vector<PerioStatus> DbPerio::getAllPerioStatuses(long long patientRowId)
{
    std::vector<PerioStatus> result;


    std::string query = "SELECT rowid, lpk, day, month, year, data FROM periostatus WHERE"
        " patient_rowid = " + std::to_string(patientRowId) +
        " ORDER BY year ASC, month ASC, day ASC";

    for (Db db(query); db.hasRows();)
    {
        result.emplace_back();

        auto& perioStatus = result.back();

        perioStatus.rowid = db.asRowId(0);
        perioStatus.LPK = db.asString(1);
        perioStatus.date = Date(
            db.asInt(2),
            db.asInt(3),
            db.asInt(4)
        );

        Parser::parse(db.asString(5), perioStatus);
    }

    return result;

}

void DbPerio::insertPerioStatus(PerioStatus& perioStatus, long long patientRowId)
{

    std::string query =
        "INSERT INTO periostatus (lpk, rzi, day, month, year, patient_rowid, data) VALUES ("
        "'" + UserManager::currentUser().doctor.LPK + "',"
        "'" + UserManager::currentUser().practice.rziCode + "',"
        + std::to_string(perioStatus.date.day) + ","
        + std::to_string(perioStatus.date.month) + ","
        + std::to_string(perioStatus.date.year) + ","
        + std::to_string(patientRowId) + ",'"
        + Parser::write(perioStatus) + "')";

    Db db;
    db.execute(query);

    perioStatus.rowid = db.lastInsertedRowID();
}

void DbPerio::updatePerioStatus(const PerioStatus& perioStatus)
{
    Db::crudQuery(

        "UPDATE periostatus SET"
        " day = " + std::to_string(perioStatus.date.day) +
        ", month = " + std::to_string(perioStatus.date.month) +
        ", year = " + std::to_string(perioStatus.date.year) +
        ", data = '" + Parser::write(perioStatus) + "' "
        "WHERE rowid = " + std::to_string(perioStatus.rowid)

    );

}


