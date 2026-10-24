#include "DbSickLeave.h"
#include "Database/Database.h"
#include "Model/User.h"

long long DbSickLeave::getNumber()
{
    auto [next, last] = User::practice().sickLeaveNumbers;
    if (next <= 0 || last < next) return 0;

    Db db(
        "SELECT DISTINCT number FROM sick_leave "
        "WHERE number BETWEEN ? AND ? "
        "ORDER BY number"
    );

    db.bind(1, next);
    db.bind(2, last);

    while (db.hasRows())
    {
        if (db.asLongLong(0) != next) break;

        if (next == last) return 0;

        ++next;
    }

    return next;
}

long long DbSickLeave::getFreeNumberCount(long long first, long long last)
{
    if (first <= 0 || last < first) return 0;

    Db db(
        "SELECT COUNT(DISTINCT number) FROM sick_leave "
        "WHERE number BETWEEN ? AND ?"
    );

    db.bind(1, first);
    db.bind(2, last);

    if (db.hasRows()) {
        return last - first + 1 - db.asLongLong(0);
    }

    return 0;
}

std::vector<SickLeave> DbSickLeave::getSickLeave(long long ambListId, Db& db)
{
    db.newStatement(
        "SELECT rowid, number, date, is_continuation, regime_type, "
        "start_date, end_date, diagnosis, reason_code, employer_name, "
        "employer_city, profession, position, note_code, additional_note, is_cancelled, checkup_date, caregiver_rowid, relationship "
        "FROM sick_leave WHERE amblist_rowid=? ORDER BY rowid"
    );

    db.bind(1, ambListId);

    std::vector<SickLeave> result;

    while (db.hasRows())
    {
        result.emplace_back();
        auto& s = result.back();

        s.rowid = db.asRowId(0);
        s.number = db.asLongLong(1);
        s.date = db.asString(2);
        s.isContinuation = db.asBool(3);
        s.regimeType = db.asInt(4);
        s.startDate = db.asString(5);
        s.endDate = db.asString(6);
        s.diagnosis = db.asString(7);
        s.reasonCode = db.asInt(8);
        s.employerName = db.asString(9);
        s.employerCity = Ekatte(db.asInt(10));
        s.profession = db.asString(11);
        s.position = db.asString(12);
        s.noteCode = db.asInt(13);
        s.additionalNote = db.asString(14);
        s.isCancelled = db.asBool(15);
        s.checkup = db.asString(16);
        s.caregiver_rowid = db.asRowId(17);
        s.relationship = db.asInt(18);
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
            "start_date, end_date, diagnosis, reason_code, employer_name, "
            "employer_city, profession, position, note_code, additional_note, is_cancelled, checkup_date, caregiver_rowid, relationship) "
            "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
        );

        db.bind(1, ambListId);
        db.bind(2, s.number);
        db.bind(3, s.date.to8601());
        db.bind(4, s.isContinuation);
        db.bind(5, s.regimeType);
        db.bind(6, s.startDate.to8601());
        db.bind(7, s.endDate.to8601());
        db.bind(8, s.diagnosis.code());
        db.bind(9, s.reasonCode);
        db.bind(10, s.employerName);
        db.bind(11, s.employerCity.getIdxAsInt());
        db.bind(12, s.profession);
        db.bind(13, s.position);
        db.bind(14, s.noteCode);
        db.bind(15, s.additionalNote);
        db.bind(16, s.isCancelled);
        db.bind(17, s.checkup.to8601());
        if (s.caregiver_rowid) {
            db.bind(18, s.caregiver_rowid);
            db.bind(19, s.relationship);
        }
        else {
            db.bindNull(18);
            db.bindNull(19);
        }

        db.execute();
    }
}
