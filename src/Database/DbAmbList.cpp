#include "DbAmbList.h"



DbAmbList::DbAmbList()
{}

std::string DbAmbList::getOlderStatus(std::string patientID)
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

std::vector<Manipulation> DbAmbList::getOlderManipulations(std::string patientID)
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

    if (amb_id.empty()) return std::vector<Manipulation>{};

    return db_manipulation.getManipulations(amb_id, date);
}


void DbAmbList::insertAmbList(AmbList& ambList, std::string &patientID)
{
    openConnection();

    std::string query = "INSERT INTO amblist (day, month, year, num, status_json, patient_id, lpk) VALUES ('"
        + std::to_string(ambList.date.day) + "','"
        + std::to_string(ambList.date.month) + "','"
        + std::to_string(ambList.date.year) + "','"
        + std::to_string(ambList.number) + "','"
        + parser.write(ambList.teeth) + "','"
        + patientID + "','"
        + ambList.LPK
        + "')";
    qDebug() << query.c_str();

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    ambList.id = std::to_string((int)sqlite3_last_insert_rowid(db));

    if (rc != SQLITE_OK) std::cout << "Insert error:" << &db;

    closeConnection();

    db_manipulation.saveManipulations(ambList.id, ambList.manipulations);

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
        ", status_json = '" + parser.write(ambList.teeth) + "' "
        "WHERE id = " + ambList.id;

    qDebug() << query.c_str();
    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) qDebug() << "Update error:";// << &db;

    closeConnection();

    db_manipulation.saveManipulations(ambList.id, ambList.manipulations);
}


AmbList* DbAmbList::getList(std::string patientID, int currentMonth, int currentYear)
{
    openConnection();

    std::string query = "SELECT id, num, day, month, year, status_json FROM amblist WHERE "
        "patient_id = '" + patientID + "' AND "
        "month = " + std::to_string(currentMonth) + " AND "
        "year = " + std::to_string(currentYear);

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    AmbList* ambList = new AmbList;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        ambList->id = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        ambList->number = sqlite3_column_int(stmt, 1);
        ambList->date.day = sqlite3_column_int(stmt, 2);
        ambList->date.month = sqlite3_column_int(stmt, 3);
        ambList->date.year = sqlite3_column_int(stmt, 4);
        ambList->test = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
    }

    sqlite3_finalize(stmt);

    closeConnection();

    parser.parse(getOlderStatus(patientID), ambList->teeth);
    ambList->test = getOlderStatus(patientID);

    if (ambList->id.empty()) //if the amblist is new
    {
        ambList->date = Date::getCurrentDate();
        m_applier.applyManipulations(getOlderManipulations(patientID), ambList->teeth, "220008771");
    }
    else
    {
        parser.parse(ambList->test, ambList->teeth);
        ambList->LPK = "220008771";
        ambList->manipulations = db_manipulation.getManipulations(ambList->id, ambList->date);
    }
   
    return ambList;

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

bool DbAmbList::checkExistingAmbNum(const int& currentYear, const int& ambNum)
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

std::map<int, bool> DbAmbList::getExistingNumbers(const int& currentYear)
{
    openConnection();

    std::map<int, bool> numbersMap;

    std::string query = "SELECT num FROM amblist WHERE lpk = '220008771' "
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


