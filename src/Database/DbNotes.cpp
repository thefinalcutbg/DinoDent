#include "DbNotes.h"

std::string DbNotes::getNote(const std::string& patientID, int toothIdx)
{
    openConnection();

    std::string query = 
        "SELECT text FROM note WHERE "
        "patient_id = '" + patientID + "' AND "
        "tooth = " + std::to_string(toothIdx);

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    std::string result;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }

    sqlite3_finalize(stmt);

    closeConnection();

    return result;
}

void DbNotes::saveNote(const std::string& note, const std::string& patientID, int toothIdx)
{
    openConnection();

    std::string query = "INSERT INTO note(patient_id, tooth, text) "
        "VALUES('" + patientID + "', " + std::to_string(toothIdx) + ",'" + note + "') "
        "ON CONFLICT(patient_id, tooth) DO UPDATE SET text = excluded.text";

    if (note.empty())
    {
        query = "DELETE FROM note WHERE "
            "patient_id = '" + patientID + "' "
            "AND tooth = " + std::to_string(toothIdx);
    }

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {}// << &db;

    closeConnection();
}
