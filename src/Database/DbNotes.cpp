#include "DbNotes.h"
#include "Database.h"

std::string DbNotes::getNote(const std::string& patientID, int toothIdx)
{
 
    std::string query = 
        "SELECT text FROM note WHERE "
        "patient_id = '" + patientID + "' AND "
        "tooth = " + std::to_string(toothIdx);

    std::string result;

    for(Db db(query); db.hasRows();)
    {
        result = db.asString(0);
    }

    return result;
}

void DbNotes::saveNote(const std::string& note, const std::string& patientID, int toothIdx)
{

    if (note.empty())
    {
        Db::crudQuery(
            
            "DELETE FROM note WHERE "
            "patient_id = '" + patientID + "' "
            "AND tooth = " + std::to_string(toothIdx)
        );

        return;
    }


    std::string query = "INSERT INTO note(patient_id, tooth, text) "
        "VALUES('" + patientID + "', " + std::to_string(toothIdx) + ",'" + note + "') "
        "ON CONFLICT(patient_id, tooth) DO UPDATE SET text = excluded.text";

    Db::crudQuery(query);
}
