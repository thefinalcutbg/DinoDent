#include "DbAmbList.h"
#include "Model/User/User.h"
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/Date.h"

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
        jsonStatus = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)); //manipulation applier should be here
    }

    qDebug() << "no such status found";

    sqlite3_finalize(stmt);

    closeConnection();

    return jsonStatus;
}

std::vector<Procedure> DbAmbList::getOlderManipulations(std::string patientID)
{
    openConnection();

    std::string query = "SELECT id, day, month, year FROM amblist WHERE "
        "patient_id = '" + patientID + "'"
        " ORDER BY id DESC LIMIT 1";
    qDebug() << "getting older manipulations";
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

    return db_manipulation.getManipulations(amb_id, date);
}


void DbAmbList::insertAmbList(AmbList& ambList, std::string &patientID)
{
    openConnection();

    std::string query = "INSERT INTO amblist (day, month, year, num, unfavourable, charge, status_json, patient_id, lpk) VALUES ('"
        + std::to_string(ambList.date.day) + "','"
        + std::to_string(ambList.date.month) + "','"
        + std::to_string(ambList.date.year) + "','"
        + std::to_string(ambList.number) + "','"
        + std::to_string(ambList.full_coverage) + "','"
        + std::to_string(static_cast<int>(ambList.charge)) + "','"
        + procedureParser.write(ambList.teeth) + "','"
        + patientID + "','"
        + ambList.LPK
        + "')";
    qDebug() << query.c_str();

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    ambList.id = std::to_string((int)sqlite3_last_insert_rowid(db));

    if (rc != SQLITE_OK) std::cout << "Insert error:" << &db;

    closeConnection();

    db_manipulation.saveManipulations(ambList.id, ambList.procedures);

}

void DbAmbList::updateAmbList(AmbList& ambList)
{
    openConnection();

    std::string query = "UPDATE amblist SET"
        " num = " + std::to_string(ambList.number) +
        ", day = " + std::to_string(ambList.date.day) +
        ", month = " + std::to_string(ambList.date.month) +
        ", year = " + std::to_string(ambList.date.year) +
        ", num = " + std::to_string(ambList.number) +
        ", unfavourable = " + std::to_string(ambList.full_coverage) +
        ", charge = " + std::to_string(static_cast<int>(ambList.charge)) +
        ", lpk = '" + CurrentUser::instance().LPK + "' " +
        ", status_json = '" + procedureParser.write(ambList.teeth) + "' "
        "WHERE id = " + ambList.id;

    qDebug() << query.c_str();
    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) qDebug() << "Update error:";// << &db;

    closeConnection();

    db_manipulation.saveManipulations(ambList.id, ambList.procedures);
}

std::vector<AmbListRow> DbAmbList::getAmbListRows(const Date& from, const Date& to)
{
    openConnection();

    std::vector<AmbListRow> rows;
    rows.reserve(50);

    std::string query =
        "SELECT amblist.id, amblist.num, amblist.day, amblist.month, amblist.year, patient.fname, patient.mname, patient.lname, patient.id "
        "FROM amblist INNER JOIN patient ON amblist.patient_id = patient.id "
        "WHERE (amblist.year, amblist.month, amblist.day) "
        "BETWEEN (" + std::to_string(from.year) + ", " + std::to_string(from.month) + ", " + std::to_string(from.day) + ") "
        "AND (" + std::to_string(to.year) + ", " + std::to_string(to.month) + ", " + std::to_string(to.day) + ") "
        "AND amblist.lpk = '" + CurrentUser::instance().LPK + "' "
        "ORDER BY amblist.year ASC, amblist.month ASC, amblist.day ASC ";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        rows.emplace_back();
        auto& row = rows.back();

       //amb list data:
        row.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        row.ambNumber = sqlite3_column_int(stmt, 1);
        row.date = Date{ sqlite3_column_int(stmt, 2), sqlite3_column_int(stmt, 3), sqlite3_column_int(stmt, 4) };

        //patient data:
        row.patientName = std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5))} + " ";
        std::string mname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        if (!mname.empty()) //some foreigners dont have middle names
            row.patientName.append(mname + " ");
        row.patientName.append(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));

        row.patientId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
    }

    closeConnection();

    return rows;
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

    closeConnection();

    return years;
}


void DbAmbList::getListData(const std::string& patientID, int month, int year, AmbList& ambList)
{
    openConnection();

    std::string query = "SELECT id, num, unfavourable, day, month, year, status_json, charge FROM amblist WHERE "
        "patient_id = '" + patientID + "' AND "
        "month = " + std::to_string(month) + " AND "
        "year = " + std::to_string(year);
    qDebug() << QString::fromStdString(query);
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);


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
        ambList.LPK = CurrentUser::instance().LPK;
    }
    
    sqlite3_finalize(stmt);

    closeConnection();

    if (ambList.isNew())
    {
        procedureParser.parse(getLastStatus(patientID), ambList.teeth);
        m_applier.applyProcedures(getOlderManipulations(patientID), ambList.teeth, CurrentUser::instance().LPK);
        ambList.LPK = CurrentUser::instance().LPK;
    }
    else
    {
        procedureParser.parse(status_json, ambList.teeth);
        ambList.procedures = db_manipulation.getManipulations(ambList.id, ambList.date);
    }

}

void DbAmbList::getListData(const std::string& ambID, AmbList& ambList)
{
    openConnection();

    std::string query = "SELECT id, num, unfavourable, day, month, year, status_json, charge FROM amblist WHERE "
        "id = '" + ambID + "'";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);


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
        ambList.LPK = CurrentUser::instance().LPK;
    }

    sqlite3_finalize(stmt);

    closeConnection();

    procedureParser.parse(status_json, ambList.teeth);
    ambList.procedures = db_manipulation.getManipulations(ambList.id, ambList.date);

}

void DbAmbList::deleteAmbList(const std::string& ambID)
{
    openConnection();

    std::string query = "DELETE FROM amblist WHERE id = " + ambID;

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    closeConnection();
}


int DbAmbList::getNewNumber(const int& currentYear)
{
    openConnection();

    std::string query = "SELECT MAX(num) FROM amblist WHERE year = " + std::to_string(currentYear);

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    int number = 0;

    while (sqlite3_step(stmt) != SQLITE_DONE)
        number = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);

    closeConnection();

    return number + 1;

}

bool DbAmbList::checkExistingAmbNum(int currentYear, int ambNum)
{
    openConnection();

    std::string query = "SELECT EXISTS(SELECT 1 FROM amblist WHERE "
        "year = " + std::to_string(currentYear) +
        " AND num = " + std::to_string(ambNum) + ")";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    bool exists = 0;

    while (sqlite3_step(stmt) != SQLITE_DONE)
        exists = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);

    closeConnection();

    return exists;

 
}

std::map<int, bool> DbAmbList::getExistingNumbers(int currentYear)
{
    openConnection();

    std::map<int, bool> numbersMap;

    std::string query = "SELECT num FROM amblist WHERE lpk = '"+ CurrentUser::instance().LPK+"' "
        "AND year = " + std::to_string(currentYear);

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE) {

        numbersMap[sqlite3_column_int(stmt, 0)] = true;
    }


    numbersMap[0] = true;

    sqlite3_finalize(stmt);

    closeConnection();

    return numbersMap;
}


