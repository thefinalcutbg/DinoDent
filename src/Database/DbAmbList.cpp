#include "DbAmbList.h"
#include "Database/Database.h"
#include "Model/User/User.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/Date.h"
#include "Model/Parser/Parser.h"
#include "DbProcedure.h"
#include <qdebug.h>
long long DbAmbList::insert(const AmbList& ambList, long long patientRowId)
{
  
    auto ambSheetDate = ambList.getDate();

    std::string query = "INSERT INTO amblist (date, num, nhif, status, patient_rowid, lpk, rzi) "
        "VALUES ("
        "'" + ambList.getDate().to8601() + "T00:00:00" + "',"
        + std::to_string(ambList.number) + ", "
        "'" + Parser::write(ambList.nhifData, ambList.hasNZOKProcedure()) + "',"
        "'" + Parser::write(ambList.teeth) + "',"
        + std::to_string(patientRowId) + ","
        "'" + ambList.LPK + "',"
        "'" + User::practice().rziCode +"')";

    Db db;

    db.execute(query);

    auto rowID = db.lastInsertedRowID();

    DbProcedure::saveProcedures(rowID, ambList.procedures.list(), &db);

    return rowID;

}

void DbAmbList::update(const AmbList& ambList)
{

    auto ambSheetDate = ambList.getDate();

    std::string query = "UPDATE amblist SET "
        "num=" + std::to_string(ambList.number) + ","
        "date='" + ambList.getDate().to8601() + "T00:00:00" + "',"
        "nhif='" + Parser::write(ambList.nhifData, ambList.hasNZOKProcedure()) + "',"
        "status = '" + Parser::write(ambList.teeth) + "' "
        "WHERE rowid = " + std::to_string(ambList.rowid);

    Db db;
    db.execute(query);

    DbProcedure::saveProcedures(ambList.rowid, ambList.procedures.list(), &db);
}

AmbList DbAmbList::getNewAmbSheet(long long patientRowId)
{

    AmbList ambList;
    ambList.LPK = User::doctor().LPK;
    std::string status;

    Db db;
    std::string query(
    
        "SELECT rowid, num, nhif, status FROM amblist WHERE "
        "patient_rowid = " + std::to_string(patientRowId) + " AND "
        "lpk = '" + User::doctor().LPK + "' AND "
        "rzi = '" + User::practice().rziCode + "' AND "
        "strftime('%Y',amblist.date) = strftime('%Y',date('now')) AND  strftime('%m',amblist.date) = strftime('%m',date('now'))"
    
    );

    qDebug() << query.data();
    db.newStatement(query);

    while(db.hasRows())
    {
        ambList.patient_rowid = patientRowId;
        ambList.rowid = db.asRowId(0);
        ambList.number = db.asInt(1);
        ambList.nhifData = Parser::parseNhifData(db.asString(2));
        status = db.asString(3);
    }

    if (ambList.isNew())
    {
        db.newStatement(
            
            "SELECT rowid, status FROM amblist WHERE "
            "patient_rowid = " + std::to_string(patientRowId) + " "
            "ORDER BY date DESC LIMIT 1"

            );

        long long oldId = 0;

        while(db.hasRows()){
            
            oldId = db.asRowId(0);
            status = db.asString(1);
        }

        if (!oldId) return ambList; //it means no data is found for this patient

        Parser::parse(status, ambList.teeth);

        auto procedures = DbProcedure::getProcedures(oldId, &db);

        for (auto& p : procedures)
        {
            p.applyProcedure(ambList.teeth);
        }
    }
    else
    {
        Parser::parse(status, ambList.teeth);
        ambList.procedures.addProcedures(DbProcedure::getProcedures(ambList.rowid, &db));
    }

    return ambList;
}

AmbList DbAmbList::getListData(long long rowId)
{

    std::string status;
    AmbList ambList;

    Db db(
        "SELECT rowid, num, nhif, status, patient_rowid FROM amblist WHERE "
        "rowid = " + std::to_string(rowId)
    );

    while (db.hasRows())
    {
        ambList.rowid = db.asRowId(0);
        ambList.number = db.asInt(1);
        ambList.nhifData = Parser::parseNhifData(db.asString(2));
        status = db.asString(3);
        ambList.LPK = User::doctor().LPK;
        ambList.patient_rowid = db.asRowId(4);
    }

    Parser::parse(status, ambList.teeth);
    ambList.procedures.addProcedures(DbProcedure::getProcedures(ambList.rowid, &db));

    return ambList;
}


void DbAmbList::deleteCurrentSelection(const std::string& ambID)
{
    Db::crudQuery("DELETE FROM amblist WHERE rowid = " + ambID + ")");
}


bool DbAmbList::checkExistingAmbNum(int currentYear, int ambNum)
{
    std::string query = "SELECT EXISTS(SELECT 1 FROM amblist WHERE "
        "strftime('%Y',date)=" + std::to_string(currentYear) + " "
        "AND num = " + std::to_string(ambNum) + ") "
        "AND lpk = '" + User::doctor().LPK + "' "
        "AND rzi = '" + User::practice().rziCode + "' ";

    bool exists = 0;

    for (Db db(query); db.hasRows();)
        exists = db.asInt(0);

    return exists;
}

std::unordered_set<int> DbAmbList::getExistingNumbers(int currentYear)
{

    std::unordered_set<int> existingNumbers;

    std::string query = "SELECT num FROM amblist WHERE "
        "lpk = '" + User::doctor().LPK + "' "
        "AND rzi = '" + User::practice().rziCode + "' "
        "AND strftime('%Y',date)=" + std::to_string(currentYear);

    for (Db db(query);db.hasRows();) existingNumbers.emplace(db.asInt(0));

    return existingNumbers;
}

bool DbAmbList::suchNumberExists(int year, int ambNum, long long ambRowid)
{

    Db db{
        "SELECT COUNT(num) FROM amblist WHERE "
        "lpk = '" + User::doctor().LPK + "' "
        "AND rzi ='" + User::practice().rziCode + "' "
        "AND strftime('%Y',date)=" + std::to_string(year) + " "
        "AND num =" + std::to_string(ambNum) + " "
        "AND rowid !=" + std::to_string(ambRowid)
    };

    for (;db.hasRows();) {
        return db.asBool(0);
    }
   
    return false;

}

std::vector<long long> DbAmbList::getRowIdNhif(int month, int year)
{

    std::string query = 
        "SELECT amblist.rowid FROM amblist "
        "JOIN procedure ON amblist.rowid = procedure.amblist_rowid "
        "GROUP BY amblist.rowid "
        "HAVING "
        "lpk = '" + User::doctor().LPK + "' "
        "AND rzi = '" + User::practice().rziCode + "' "
        "AND sum(procedure.nzok) > 0 "
        "AND strftime('%m',amblist.date)=" + std::to_string(month) + " "
        "AND strftime('%Y',amblist.date)=" + std::to_string(year) + " "
        "ORDER BY amblist.num ASC";

    std::vector<long long> result;

    for(Db db(query);db.hasRows();) result.push_back(db.asRowId(0));

    return result;
}

AmbList DbAmbList::getListNhifProceduresOnly(long long rowId)
{

    std::string status;
    AmbList ambList;

    Db db(
        "SELECT rowid, num, nhif, status, patient_rowid FROM amblist WHERE "
        "rowid = " + std::to_string(rowId)
    );

    while (db.hasRows())
    {
        ambList.rowid = db.asRowId(0);
        ambList.number = db.asInt(1);
        ambList.nhifData = Parser::parseNhifData(db.asString(2));
        status = db.asString(3);
        ambList.LPK = User::doctor().LPK;
        ambList.patient_rowid = db.asRowId(4);
    }

    Parser::parse(status, ambList.teeth);
    ambList.procedures.addProcedures(DbProcedure::getProcedures(ambList.rowid, &db, true));

    return ambList;
}

int DbAmbList::getNewNumber(Date ambDate, bool nzok)
{

    std::string query;

    std::string condition = nzok ? "sum(procedure.nzok) > 0 " : "sum(procedure.nzok) = 0 ";

    query = 
        "SELECT amblist.num FROM amblist "
        "JOIN procedure ON amblist.rowid = procedure.amblist_rowid "
        "GROUP BY amblist.rowid "
        "HAVING "
        + condition +
        "AND strftime('%Y-%m-%d', amblist.date) BETWEEN '" + std::to_string(ambDate.year) + "-01-01'"
        "AND '" + ambDate.to8601() + "' "
        "AND amblist.lpk = '" + User::doctor().LPK + "' "
        "AND amblist.rzi = '" + User::practice().rziCode + "' "
        "ORDER BY amblist.num DESC LIMIT 1";

    int number = nzok ? 0 : 100000;

    for (Db db(query); db.hasRows();) {
        number = db.asInt(0);
    };

    return number + 1;

}



