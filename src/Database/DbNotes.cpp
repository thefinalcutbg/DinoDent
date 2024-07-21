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

bool DbNotes::saveNote(const std::string& note, long long patientRowId, int toothIdx)
{

    Db db;

    if (note.empty())
    {
        return db.execute(
            "DELETE FROM note WHERE "
            "patient_rowid = " + std::to_string(patientRowId) + " "
            "AND tooth = " + std::to_string(toothIdx)
        );
    }

    db.newStatement(
        "INSERT INTO note(patient_rowid, tooth, text) VALUES(?,?,?) "
        "ON CONFLICT(patient_rowid, tooth) DO UPDATE SET text = excluded.text"
    );

    db.bind(1, patientRowId);
    db.bind(2, toothIdx);
    db.bind(3, note);

    return db.execute();
}
