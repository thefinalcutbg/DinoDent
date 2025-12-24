#include "DbPerio.h"
#include "Model/Parser.h"
#include "Model/Dental/Procedure.h"
#include "Database.h"
#include "DbProcedure.h"
#include "Model/User.h"

PerioStatus DbPerio::getPerioStatus(long long patientRowId, Date date)
{
    PerioStatus perioStatus;

    std::string query = "SELECT rowid, lpk, date, data FROM periostatus WHERE"
        " patient_rowid = " + std::to_string(patientRowId) +
        " AND date <= '" + date.to8601() + "' "
        " ORDER BY date DESC LIMIT 1";


    for(Db db(query); db.hasRows();)
    {

        perioStatus.rowid = db.asRowId(0);
        perioStatus.LPK = db.asString(1);
        perioStatus.date = db.asString(2);
        Parser::parse(db.asString(3), perioStatus);
    }

    return perioStatus;
        
}

PerioStatus DbPerio::getPerioStatus(long long rowid)
{
    PerioStatus perioStatus;

    Db db(
        "SELECT rowid, lpk, date, data FROM periostatus WHERE"
        " rowid = " + std::to_string(rowid));

    while (db.hasRows())
    {

        perioStatus.rowid = db.asRowId(0);
        perioStatus.LPK = db.asString(1);
        perioStatus.date = db.asString(2);

        Parser::parse(db.asString(3), perioStatus);
    }

    return perioStatus;
}

std::vector<PerioStatus> DbPerio::getAllPerioStatuses(long long patientRowId)
{
    std::vector<PerioStatus> result;


    std::string query = "SELECT rowid, lpk, date, data FROM periostatus WHERE"
        " patient_rowid = " + std::to_string(patientRowId) +
        " ORDER BY date ASC";

    for (Db db(query); db.hasRows();)
    {
        result.emplace_back();

        auto& perioStatus = result.back();

        perioStatus.rowid = db.asRowId(0);
        perioStatus.LPK = db.asString(1);
        perioStatus.date = db.asString(2);
        Parser::parse(db.asString(3), perioStatus);
    }

    return result;

}

;long long DbPerio::insertPerioStatus(const PerioStatus& perioStatus, long long patientRowId)
{

    std::string query =
        "INSERT INTO periostatus (lpk, rzi, date, patient_rowid, data) VALUES ("
        "'" + User::doctor().LPK + "',"
        "'" + User::practice().rziCode + "',"
        "'"+ perioStatus.date.to8601() + "',"
        + std::to_string(patientRowId) + ",'"
        + Parser::write(perioStatus) + "')";

    Db db;
    db.execute(query);

    return db.lastInsertedRowID();
}

void DbPerio::updatePerioStatus(const PerioStatus& perioStatus)
{
    Db::crudQuery(

        "UPDATE periostatus SET "
        "date = '" + perioStatus.date.to8601() + "', "
        "data = '" + Parser::write(perioStatus) + "' "
        "WHERE rowid = " + std::to_string(perioStatus.rowid)

    );

}


