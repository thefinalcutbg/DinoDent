#include "DbAmbList.h"
#include "Model/User/UserManager.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/Date.h"
#include "Model/Parser/Parser.h"

DbAmbList::DbAmbList()
{}

std::string DbAmbList::getLastStatus(std::string patientID)
{
    openConnection();

    std::string jsonStatus;

    std::string query = "SELECT status_json FROM amblist WHERE "
        "patient_id = '" + patientID + "'"
        " ORDER BY id DESC LIMIT 1";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        jsonStatus = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }


    sqlite3_finalize(stmt);

    closeConnection();

    return jsonStatus;
}

std::vector<Procedure> DbAmbList::previousProcedures(std::string patientID)
{
    openConnection();

    std::string query = "SELECT id, day, month, year FROM amblist WHERE "
        "patient_id = '" + patientID + "'"
        " ORDER BY id DESC LIMIT 1";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    std::string amb_id;
    Date date;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        amb_id = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        date.day = sqlite3_column_int(stmt, 1);
        date.month = sqlite3_column_int(stmt, 2);
        date.year = sqlite3_column_int(stmt, 3);
    }

    sqlite3_finalize(stmt);

    closeConnection();

    if (amb_id.empty()) return std::vector<Procedure>{};

    return db_procedures.getProcedures(amb_id);
}


std::string DbAmbList::insertAmbList(const AmbList& ambList, const std::string &patientID)
{
    openConnection();

    std::string query = "INSERT INTO amblist (day, month, year, num, fullCoverage, charge, status_json, patient_id, lpk, rzi) VALUES ('"
        + std::to_string(ambList.date.day) + "','"
        + std::to_string(ambList.date.month) + "','"
        + std::to_string(ambList.date.year) + "','"
        + std::to_string(ambList.number) + "','"
        + std::to_string(ambList.full_coverage) + "','"
        + std::to_string(static_cast<int>(ambList.charge)) + "','"
        + Parser::write(ambList.teeth) + "','"
        + patientID + "','"
        + ambList.LPK + "','"
        + UserManager::currentUser().practice.rziCode
        + "')";

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    auto rowID = std::to_string((int)sqlite3_last_insert_rowid(db));

    if (rc != SQLITE_OK) qDebug() << "Insert error:" << &db;

    closeConnection();

    db_procedures.saveProcedures(rowID, ambList.procedures);

    return rowID;

}

void DbAmbList::updateAmbList(const AmbList& ambList)
{
    openConnection();

    std::string query = "UPDATE amblist SET"
        " num = " + std::to_string(ambList.number) +
        ", day = " + std::to_string(ambList.date.day) +
        ", month = " + std::to_string(ambList.date.month) +
        ", year = " + std::to_string(ambList.date.year) +
        ", fullCoverage = " + std::to_string(ambList.full_coverage) +
        ", charge = " + std::to_string(static_cast<int>(ambList.charge)) +
        ", status_json = '" + Parser::write(ambList.teeth) + "' "
        "WHERE id = " + ambList.id;

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) qDebug() << "Update error:";// << &db;

    closeConnection();

    db_procedures.saveProcedures(ambList.id, ambList.procedures);
}

std::vector<int> DbAmbList::getValidYears()
{
    std::vector<int> years;

    openConnection();

    sqlite3_prepare_v2(db, "SELECT DISTINCT year FROM amblist ORDER BY year DESC", -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        years.emplace_back(sqlite3_column_int(stmt, 0));
    }

    sqlite3_finalize(stmt);

    closeConnection();

    return years;
}


AmbList DbAmbList::getListData(const std::string& patientID, int month, int year)
{
    openConnection();

    std::string query = "SELECT id, num, fullCoverage, day, month, year, status_json, charge FROM amblist WHERE "
        "patient_id = '" + patientID + "' AND "
        "lpk = " + UserManager::currentUser().doctor.LPK + " AND "
        "rzi = " + UserManager::currentUser().practice.rziCode + " AND "
        "month = " + std::to_string(month) + " AND "
        "year = " + std::to_string(year);

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    AmbList ambList;
    std::string status_json;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        ambList.id = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        ambList.number = sqlite3_column_int(stmt, 1);
        ambList.full_coverage = sqlite3_column_int(stmt, 2);
        ambList.date.day = sqlite3_column_int(stmt, 3);
        ambList.date.month = sqlite3_column_int(stmt, 4);
        ambList.date.year = sqlite3_column_int(stmt, 5);
        status_json = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        ambList.charge = static_cast<Charge>(sqlite3_column_int(stmt, 7));
        ambList.LPK = UserManager::currentUser().doctor.LPK;
    }
    
    sqlite3_finalize(stmt);

    closeConnection();

    if (ambList.isNew())
    {
        
        Parser::parse(getLastStatus(patientID), ambList.teeth);
        auto procedures = previousProcedures(patientID);
        for (auto& p : procedures)
        {
            p.applyProcedure(ambList.teeth);
        }

        ambList.LPK = UserManager::currentUser().doctor.LPK;
    }
    else
    {
        Parser::parse(status_json, ambList.teeth);
        ambList.procedures = db_procedures.getProcedures(ambList.id);
    }

    return ambList;
}

AmbList DbAmbList::getListData(const std::string& ambID)
{
    openConnection();

    std::string query = "SELECT id, num, fullCoverage, day, month, year, status_json, charge FROM amblist WHERE "
        "id = '" + ambID + "'";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);


    std::string status_json;
    AmbList ambList;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        ambList.id = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        ambList.number = sqlite3_column_int(stmt, 1);
        ambList.full_coverage = sqlite3_column_int(stmt, 2);
        ambList.date.day = sqlite3_column_int(stmt, 3);
        ambList.date.month = sqlite3_column_int(stmt, 4);
        ambList.date.year = sqlite3_column_int(stmt, 5);
        status_json = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        ambList.charge = static_cast<Charge>(sqlite3_column_int(stmt, 7));
        ambList.LPK = UserManager::currentUser().doctor.LPK;
    }

    sqlite3_finalize(stmt);

    closeConnection();

    Parser::parse(status_json, ambList.teeth);
    ambList.procedures = db_procedures.getProcedures(ambList.id);

    return ambList;
}


void DbAmbList::deleteCurrentSelection(const std::string& ambID)
{
    openConnection();

    std::string query = "DELETE FROM amblist WHERE id = " + ambID;

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    closeConnection();

}

#include <QDebug>


bool DbAmbList::checkExistingAmbNum(int currentYear, int ambNum)
{
    openConnection();

    std::string query = "SELECT EXISTS(SELECT 1 FROM amblist WHERE "
        "year = " + std::to_string(currentYear) +
        " AND num = " + std::to_string(ambNum) + ")"
        " AND lpk = '" + UserManager::currentUser().doctor.LPK + "' "
        " AND rzi = '" + UserManager::currentUser().practice.rziCode + "' ";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    bool exists = 0;

    while (sqlite3_step(stmt) != SQLITE_DONE)
        exists = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);

    closeConnection();

    return exists;
}

std::unordered_set<int> DbAmbList::getExistingNumbers(int currentYear)
{
    openConnection();

    std::unordered_set<int> existingNumbers;

    std::string query = "SELECT num FROM amblist WHERE " 
        "lpk = '" + UserManager::currentUser().doctor.LPK + "' "
        "AND rzi = '" + UserManager::currentUser().practice.rziCode + "' "
        "AND year = " + std::to_string(currentYear);

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE) {
        
        existingNumbers.insert(sqlite3_column_int(stmt, 0));
    }

    sqlite3_finalize(stmt);
   
    closeConnection();

    return existingNumbers;
}


int DbAmbList::getNewNumber(int currentYear)
{

    openConnection();

    std::string query = "SELECT MAX(num) FROM amblist WHERE "
        "year = " + std::to_string(currentYear) + " "
        "AND lpk = '" + UserManager::currentUser().doctor.LPK + "' "
        "AND rzi = '" + UserManager::currentUser().practice.rziCode + "'";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    int number = 0;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {

        number = sqlite3_column_int(stmt, 0); 
    }


    sqlite3_finalize(stmt);

    closeConnection();

    return number + 1;

}



