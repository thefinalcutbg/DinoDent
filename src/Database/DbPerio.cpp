#include "DbPerio.h"
#include "Model/Parser/Parser.h"
#include "Model/Procedure/Procedure.h"
#include "Database.h"
#include <QDebug>

ToothContainer DbPerio::getStatus(long long patientRowId, const Date& date)
{

    std::string jsonStatus;
    long long amblistId{0};
    std::string LPK;

    std::string query = " SELECT amblist.status_json, amblist.rowid, amblist.LPK, procedure.rowid"
                        " FROM amblist LEFT JOIN procedure ON amblist.rowid = procedure.amblist_rowid WHERE "
                        " amblist.patient_rowid = " + std::to_string(patientRowId) +
                        " AND (amblist.year, amblist.month, procedure.day) "
                        " BETWEEN (2000, 1, 1) AND ("
                         + std::to_string(date.year) + ","
                         + std::to_string(date.month) + ","
                         + std::to_string(date.day) + 
                         ")"

                        " ORDER BY amblist.year DESC, amblist.month DESC, procedure.day DESC, procedure.rowid DESC" 
                        " LIMIT 1";

    Db db(query);

    qDebug() << QString::fromStdString(query);

    long long procedureRowId{0};

    while (db.hasRows())
    {
        jsonStatus = db.asString(0);
        amblistId = db.asRowId(1);
        LPK = db.asString(2);
        procedureRowId = db.asRowId(3);
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
        " AND rowid <= " + std::to_string(procedureRowId) +
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
        " AND (year,month,day) BETWEEN (0,0,0) AND"
        " (" + std::to_string(date.year) +
        "," + std::to_string(date.month) +
        "," + std::to_string(date.day) + ")"
        " ORDER BY year DESC, month DESC, day DESC LIMIT 1";


    qDebug() << QString::fromStdString(query);

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


