#include "DbAmbList.h"
#include "Database/Database.h"
#include "Model/User/UserManager.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/Date.h"
#include "Model/Parser/Parser.h"
#include "DbProcedure.h"

std::string DbAmbList::insertAmbList(const AmbList& ambList, const std::string &patientID)
{
  
    auto ambSheetDate = ambList.getAmbListDate();

    std::string query = "INSERT INTO amblist (day, month, year, num, fullCoverage, charge, status_json, patient_id, lpk, rzi) VALUES ('"
        + std::to_string(ambSheetDate.day) + "','"
        + std::to_string(ambSheetDate.month) + "','"
        + std::to_string(ambSheetDate.year) + "','"
        + std::to_string(ambList.number) + "','"
        + std::to_string(ambList.full_coverage) + "','"
        + std::to_string(static_cast<int>(ambList.charge)) + "','"
        + Parser::write(ambList.teeth) + "','"
        + patientID + "','"
        + ambList.LPK + "','"
        + UserManager::currentUser().practice.rziCode
        + "')";

    Db db;

    db.execute(query);

    auto rowID = std::to_string(db.lastInsertedRowID());

    DbProcedure::saveProcedures(rowID, ambList.procedures, &db);

    return rowID;

}

void DbAmbList::updateAmbList(const AmbList& ambList)
{

    auto ambSheetDate = ambList.getAmbListDate();

    std::string query = "UPDATE amblist SET"
        " num = " + std::to_string(ambList.number) +
        ", day = " + std::to_string(ambSheetDate.day) +
        ", month = " + std::to_string(ambSheetDate.month) +
        ", year = " + std::to_string(ambSheetDate.year) +
        ", fullCoverage = " + std::to_string(ambList.full_coverage) +
        ", charge = " + std::to_string(static_cast<int>(ambList.charge)) +
        ", status_json = '" + Parser::write(ambList.teeth) + "' "
        "WHERE id = " + ambList.id;

    Db db;
    db.execute(query);

    DbProcedure::saveProcedures(ambList.id, ambList.procedures, &db);
}

std::vector<int> DbAmbList::getValidYears()
{
    std::vector<int> years;

    Db db("SELECT DISTINCT year FROM amblist ORDER BY year DESC");

    while (db.hasRows())
    {
        years.emplace_back(db.asInt(0));
    }

    return years;
}


AmbList DbAmbList::getNewAmbSheet(const std::string& patientID)
{

    AmbList ambList;
    ambList.LPK = UserManager::currentUser().doctor.LPK;
    std::string status_json;

    Db db(
    
        "SELECT id, num, fullCoverage, status_json, charge FROM amblist WHERE "
        "patient_id = '" + patientID + "' AND "
        "lpk = '" + UserManager::currentUser().doctor.LPK + "' AND "
        "rzi = '" + UserManager::currentUser().practice.rziCode + "' AND "
        "month = " + std::to_string(Date::currentMonth()) + " AND "
        "year = " + std::to_string(Date::currentYear())
    
    );

    while(db.hasRows())
    {
        ambList.id = db.asString(0);
        ambList.number = db.asInt(1);
        ambList.full_coverage = db.asInt(2);
        status_json = db.asString(3);
        ambList.charge = static_cast<Charge>(db.asInt(4));
    }

    if (ambList.isNew())
    {
        db.newStatement(
            
            "SELECT id, status_json FROM amblist WHERE "
            "patient_id = '" + patientID + "'"
            "ORDER BY year DESC, month DESC LIMIT 1"

            );

        std::string oldId;

        while(db.hasRows()){
            
            oldId = db.asString(0);
            status_json = db.asString(1);
        }

        if (oldId.empty()) return ambList; //it means no data is found for this patient

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
        ambList.procedures = DbProcedure::getProcedures(ambList.id, &db);
    }

    return ambList;
}

AmbList DbAmbList::getListData(const std::string& ambID)
{

    std::string status_json;
    AmbList ambList;

    Db db(
        "SELECT id, num, fullCoverage, status_json, charge FROM amblist WHERE "
        "id = '" + ambID + "'"
    );

    while (db.hasRows())
    {
        ambList.id = db.asString(0);
        ambList.number = db.asInt(1);
        ambList.full_coverage = db.asInt(2);
        status_json = db.asString(3);
        ambList.charge = static_cast<Charge>(db.asInt(4));
        ambList.LPK = UserManager::currentUser().doctor.LPK;
    }

    Parser::parse(status_json, ambList.teeth);
    ambList.procedures = DbProcedure::getProcedures(ambList.id, &db);

    return ambList;
}


void DbAmbList::deleteCurrentSelection(const std::string& ambID)
{
    Db::crudQuery("DELETE FROM amblist WHERE id = " + ambID + ")");
}


bool DbAmbList::checkExistingAmbNum(int currentYear, int ambNum)
{
    std::string query = "SELECT EXISTS(SELECT 1 FROM amblist WHERE "
        "year = " + std::to_string(currentYear) +
        " AND num = " + std::to_string(ambNum) + ")"
        " AND lpk = '" + UserManager::currentUser().doctor.LPK + "' "
        " AND rzi = '" + UserManager::currentUser().practice.rziCode + "' ";

    bool exists = 0;

    for (Db db(query); db.hasRows();)
        exists = db.asInt(0);

    return exists;
}

std::unordered_set<int> DbAmbList::getExistingNumbers(int currentYear)
{

    std::unordered_set<int> existingNumbers;

    std::string query = "SELECT num FROM amblist WHERE " 
        "lpk = '" + UserManager::currentUser().doctor.LPK + "' "
        "AND rzi = '" + UserManager::currentUser().practice.rziCode + "' "
        "AND year = " + std::to_string(currentYear);

    for (Db db(query);db.hasRows();) existingNumbers.emplace(db.asInt(0));

    return existingNumbers;
}


int DbAmbList::getNewNumber(int currentYear, bool nzok)
{

    std::string query;

    std::string condition = nzok ? "sum(procedure.nzok) > 0 " : "sum(procedure.nzok) = 0 ";

    query = 
        "SELECT amblist.num FROM amblist "
        "LEFT JOIN procedure ON amblist.id = procedure.amblist_id "
        "GROUP BY amblist.id "
        "HAVING "
        + condition +
        "AND year = " + std::to_string(currentYear) + " "
        "AND lpk = '" + UserManager::currentUser().doctor.LPK + "' "
        "AND rzi = '" + UserManager::currentUser().practice.rziCode + "' "
        "ORDER BY amblist.num DESC LIMIT 1";

    int number = 0;

    for (Db db(query); db.hasRows();) number = db.asInt(0);

    return number + 1;

}



