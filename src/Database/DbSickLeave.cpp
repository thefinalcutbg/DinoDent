#include "DbSickLeave.h"
#include "Database/Database.h"

std::vector<SickLeave> DbSickLeave::getSickLeave(long long ambListId, Db& db)
{
    db.newStatement(
        "SELECT rowid, number, date, is_continuation, regime_type, "
        "start_date, end_date, diagnosis, diagnosis_description, reason_code, employer_name, "
        "employer_city, profession, position, note_code, additional_note, is_cancelled "
        "FROM sick_leave WHERE amblist_rowid=? ORDER BY rowid"
    );

    db.bind(1, ambListId);

    std::vector<SickLeave> result;

    while (db.hasRows())
    {
        result.emplace_back();
        auto& s = result.back();

        s.rowid = db.asRowId(0);
        s.number = db.asString(1);
        s.date = db.asString(2);
        s.isContinuation = db.asBool(3);
        s.regimeType = db.asInt(4);
        s.startDate = db.asString(5);
        s.endDate = db.asString(6);
        s.diagnosis.icd = db.asString(7);
        s.diagnosis.additional_descr = db.asString(8);
        s.reasonCode = db.asInt(9);
        s.employerName = db.asString(10);
        s.employerCity = Ekatte(db.asInt(11));
        s.profession = db.asString(12);
        s.position = db.asString(13);
        s.noteCode = db.asInt(14);
        s.additionalNote = db.asString(15);
        s.isCancelled = db.asBool(16);
    }

    return result;
}

void DbSickLeave::saveSickLeave(const std::vector<SickLeave>& sickLeave, long long ambListId)
{
    Db db;
    saveSickLeave(sickLeave, ambListId, db);
}

void DbSickLeave::saveSickLeave(const std::vector<SickLeave>& sickLeave, long long ambListId, Db& db)
{
    db.newStatement("DELETE FROM sick_leave WHERE amblist_rowid=?");
    db.bind(1, ambListId);
    db.execute();

    for (const auto& s : sickLeave)
    {
        db.newStatement(
            "INSERT INTO sick_leave "
            "(amblist_rowid, number, date, is_continuation, regime_type, "
            "start_date, end_date, diagnosis, diagnosis_description, reason_code, employer_name, "
            "employer_city, profession, position, note_code, additional_note, is_cancelled) "
            "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
        );

        db.bind(1, ambListId);
        db.bind(2, s.number);
        db.bind(3, s.date.to8601());
        db.bind(4, s.isContinuation);
        db.bind(5, s.regimeType);
        db.bind(6, s.startDate.to8601());
        db.bind(7, s.endDate.to8601());
        db.bind(8, s.diagnosis.icd.code());
        db.bind(9, s.diagnosis.additional_descr);
        db.bind(10, s.reasonCode);
        db.bind(11, s.employerName);
        db.bind(12, s.employerCity.getIdxAsInt());
        db.bind(13, s.profession);
        db.bind(14, s.position);
        db.bind(15, s.noteCode);
        db.bind(16, s.additionalNote);
        db.bind(17, s.isCancelled);

        db.execute();
    }
}
