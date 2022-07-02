#include "DbAmbList.h"
#include "Database/Database.h"
#include "Model/User/User.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/Date.h"
#include "Model/Parser/Parser.h"
#include "DbProcedure.h"
#include <QDebug>
long long DbAmbList::insert(const AmbList& ambList, long long patientRowId)
{
  
    auto ambSheetDate = ambList.getDate();

    std::string query = "INSERT INTO amblist (day, month, year, num, fullCoverage, charge, status_json, patient_rowid, lpk, rzi) "
        "VALUES ("
        + std::to_string(ambSheetDate.day) + ","
        + std::to_string(ambSheetDate.month) + ","
        + std::to_string(ambSheetDate.year) + ","
        + std::to_string(ambList.number) + ","
        + std::to_string(ambList.full_coverage) + ","
        + std::to_string(static_cast<int>(ambList.charge)) + ",'"
        + Parser::write(ambList.teeth) + "',"
        + std::to_string(patientRowId) + ",'"
        + ambList.LPK + "','"
        + User::practice().rziCode
        + "')";

    Db db;

    db.execute(query);

    auto rowID = db.lastInsertedRowID();

    DbProcedure::saveProcedures(rowID, ambList.procedures, &db);

    return rowID;

}

void DbAmbList::update(const AmbList& ambList)
{

    auto ambSheetDate = ambList.getDate();

    std::string query = "UPDATE amblist SET"
        " num = " + std::to_string(ambList.number) +
        ", day = " + std::to_string(ambSheetDate.day) +
        ", month = " + std::to_string(ambSheetDate.month) +
        ", year = " + std::to_string(ambSheetDate.year) +
        ", fullCoverage = " + std::to_string(ambList.full_coverage) +
        ", charge = " + std::to_string(static_cast<int>(ambList.charge)) +
        ", status_json = '" + Parser::write(ambList.teeth) + "' "
        "WHERE rowid = " + std::to_string(ambList.rowid);

    Db db;
    db.execute(query);

    DbProcedure::saveProcedures(ambList.rowid, ambList.procedures, &db);
}

AmbList DbAmbList::getNewAmbSheet(long long patientRowId)
{

    AmbList ambList;
    ambList.LPK = User::doctor().LPK;
    std::string status_json;

    Db db(
    
        "SELECT rowid, num, fullCoverage, status_json, charge FROM amblist WHERE "
        "patient_rowid = " + std::to_string(patientRowId) + " AND "
        "lpk = '" + User::doctor().LPK + "' AND "
        "rzi = '" + User::practice().rziCode + "' AND "
        "month = " + std::to_string(Date::currentMonth()) + " AND "
        "year = " + std::to_string(Date::currentYear())
    
    );

    while(db.hasRows())
    {
        ambList.patient_rowid = patientRowId;
        ambList.rowid = db.asRowId(0);
        ambList.number = db.asInt(1);
        ambList.full_coverage = db.asInt(2);
        status_json = db.asString(3);
        ambList.charge = static_cast<Charge>(db.asInt(4));
    }

    if (ambList.isNew())
    {
        db.newStatement(
            
            "SELECT rowid, status_json FROM amblist WHERE "
            "patient_rowid = " + std::to_string(patientRowId) + " "
            "ORDER BY year DESC, month DESC LIMIT 1"

            );

        long long oldId = 0;

        while(db.hasRows()){
            
            oldId = db.asRowId(0);
            status_json = db.asString(1);
        }

        if (!oldId) return ambList; //it means no data is found for this patient

        Parser::parse(status_json, ambList.teeth);

        auto procedures = DbProcedure::getProcedures(oldId, &db);

        for (auto& p : procedures)
        {
            p.applyProcedure(ambList.teeth);
        }
    }
    else
    {
        Parser::parse(status_json, ambList.teeth);
        ambList.procedures = DbProcedure::getProcedures(ambList.rowid, &db);
    }

    return ambList;
}

AmbList DbAmbList::getListData(long long rowId)
{

    std::string status_json;
    AmbList ambList;

    Db db(
        "SELECT rowid, num, fullCoverage, status_json, charge, patient_rowid FROM amblist WHERE "
        "rowid = " + std::to_string(rowId)
    );

    while (db.hasRows())
    {
        ambList.rowid = db.asRowId(0);
        ambList.number = db.asInt(1);
        ambList.full_coverage = db.asInt(2);
        status_json = db.asString(3);
        ambList.charge = static_cast<Charge>(db.asInt(4));
        ambList.LPK = User::doctor().LPK;
        ambList.patient_rowid = db.asRowId(5);
    }

    Parser::parse(status_json, ambList.teeth);
    ambList.procedures = DbProcedure::getProcedures(ambList.rowid, &db);

    return ambList;
}


void DbAmbList::deleteCurrentSelection(const std::string& ambID)
{
    Db::crudQuery("DELETE FROM amblist WHERE rowid = " + ambID + ")");
}


bool DbAmbList::checkExistingAmbNum(int currentYear, int ambNum)
{
    std::string query = "SELECT EXISTS(SELECT 1 FROM amblist WHERE "
        "year = " + std::to_string(currentYear) +
        " AND num = " + std::to_string(ambNum) + ")"
        " AND lpk = '" + User::doctor().LPK + "' "
        " AND rzi = '" + User::practice().rziCode + "' ";

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
        "AND year = " + std::to_string(currentYear);

    for (Db db(query);db.hasRows();) existingNumbers.emplace(db.asInt(0));

    return existingNumbers;
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
        "AND month = " + std::to_string(month) + " "
        "AND year = " + std::to_string(year) + " "
        "ORDER BY amblist.num ASC";

    std::vector<long long> result;

    for(Db db(query);db.hasRows();) result.push_back(db.asRowId(0));

    return result;
}

AmbList DbAmbList::getListNhifProceduresOnly(long long rowId)
{

    std::string status_json;
    AmbList ambList;

    Db db(
        "SELECT rowid, num, fullCoverage, status_json, charge, patient_rowid FROM amblist WHERE "
        "rowid = " + std::to_string(rowId)
    );

    while (db.hasRows())
    {
        ambList.rowid = db.asRowId(0);
        ambList.number = db.asInt(1);
        ambList.full_coverage = db.asInt(2);
        status_json = db.asString(3);
        ambList.charge = static_cast<Charge>(db.asInt(4));
        ambList.LPK = User::doctor().LPK;
        ambList.patient_rowid = db.asRowId(5);
    }

    Parser::parse(status_json, ambList.teeth);
    ambList.procedures = DbProcedure::getProcedures(ambList.rowid, &db, true);

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
        "AND amblist.year = " + std::to_string(ambDate.year) + " "
        "AND amblist.month <= " + std::to_string(ambDate.month) + " "
//        "AND amblist.day <= " + std::to_string(ambDate.day) + " "
        "AND (amblist.year, amblist.month, amblist.day) BETWEEN "
        "("+ std::to_string(ambDate.year) + ", 1, 1) "
        "AND ("
        + std::to_string(ambDate.year) + ", "
        + std::to_string(ambDate.month) + ", "
        + std::to_string(ambDate.day) + ") "
        "AND amblist.lpk = '" + User::doctor().LPK + "' "
        "AND amblist.rzi = '" + User::practice().rziCode + "' "
        "ORDER BY amblist.num DESC LIMIT 1";

    qDebug() << query.c_str();

    int number = nzok ? 0 : 100000;

    for (Db db(query); db.hasRows();) {
        number = db.asInt(0);
    };

    return number + 1;

}



