#include "DbAmbList.h"



DbAmbList::DbAmbList(Database* database) :
	db(database->db),
	err(NULL),
	stmt(NULL),
	rc(0)
{}

std::string DbAmbList::getOlderStatus(std::string patientID)
{

    std::string query = "SELECT test FROM amblist WHERE "
        "patient_id = '" + patientID + "'"
        " ORDER BY id DESC LIMIT 1";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        return std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))); //manipulation applier should be here
    }

    qDebug() << "no such status found";

    return std::string();
}


void DbAmbList::insertAmbList(AmbList& ambList, std::string &patientID)
{
    std::string query = "INSERT INTO amblist (day, month, year, num, test, patient_id, lpk) VALUES ('"
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

    ambList.id = (int)sqlite3_last_insert_rowid(db);

    if (rc != SQLITE_OK) std::cout << "Insert error:" << &db;

}

void DbAmbList::updateAmbList(AmbList& ambList)
{
    std::string query = "UPDATE amblist SET"
        " num = " + std::to_string(ambList.number) +
        ", day = " + std::to_string(ambList.date.day) +
        ", month = " + std::to_string(ambList.date.month) +
        ", year = " + std::to_string(ambList.date.year) +
        ", num = " + std::to_string(ambList.number) +
        ", test = '" + parser.write(ambList.teeth) + "' "
        "WHERE id = " + ambList.id;

    qDebug() << query.c_str();
    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) qDebug() << "Update error:";// << &db;
}


AmbList* DbAmbList::getList(std::string patientID, int currentMonth, int currentYear)
{
    std::string query = "SELECT id, num, day, month, year, test FROM amblist WHERE "
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


    if (ambList->id.empty())
    {
        ambList->date = Date::getCurrentDate();
        ambList->test = getOlderStatus(patientID);
    }

    parser.parse(ambList->test, ambList->teeth);

    ambList->LPK = "220008771";

    sqlite3_finalize(stmt);

    return ambList;

}


int DbAmbList::getNewNumber(const int& currentYear)
{
    std::string query = "SELECT MAX(num) FROM amblist WHERE year = " + std::to_string(currentYear);

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    int number = 0;

    while (sqlite3_step(stmt) != SQLITE_DONE)
        number = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);

    return number + 1;
}

bool DbAmbList::checkExistingAmbNum(const int& currentYear, const int& ambNum)
{
    std::string query = "SELECT EXISTS(SELECT 1 FROM amblist WHERE "
        "year = " + std::to_string(currentYear) +
        " AND num = " + std::to_string(ambNum) + ")";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    bool exists = 0;

    while (sqlite3_step(stmt) != SQLITE_DONE)
        exists = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);

    return exists;
}

std::map<int, bool> DbAmbList::getExistingNumbers(const int& currentYear)
{
    std::map<int, bool> numbersMap;

    std::string query = "SELECT num FROM amblist WHERE lpk = '220008771' "
        "AND year = " + std::to_string(currentYear);

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE) {

        numbersMap[sqlite3_column_int(stmt, 0)] = true;
    }


    numbersMap[0] = true;

    return numbersMap;
}


