#include "DbNotes.h"
#include "Database.h"

std::string DbNotes::getNote(long long patientRowId, int toothIdx)
{
 
    std::string query = 
        "SELECT text FROM note WHERE "
        "patient_rowid = " + std::to_string(patientRowId) + " AND "
        "tooth = " + std::to_string(toothIdx);

    std::string result;

    for(Db db(query); db.hasRows();)
    {
        result = db.asString(0);
    }

    return result;
}

void DbNotes::saveNote(const std::string& note, long long patientRowId, int toothIdx)
{

    if (note.empty())
    {
        Db::crudQuery(
            
            "DELETE FROM note WHERE "
            "patient_rowid = " + std::to_string(patientRowId) + " "
            "AND tooth = " + std::to_string(toothIdx)
        );

        return;
    }


    std::string query = "INSERT INTO note(patient_rowid, tooth, text) "
        "VALUES(" + std::to_string(patientRowId) + ", " + std::to_string(toothIdx) + ",'" + note + "') "
        "ON CONFLICT(patient_rowid, tooth) DO UPDATE SET text = excluded.text";

    Db::crudQuery(query);
}
