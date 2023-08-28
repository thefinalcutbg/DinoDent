#include "DbMedicalNotice.h"
#include <Database/Database.h>

std::vector<MedicalNotice> DbMedicalNotice::get(long long amblistRowid, Db& db)
{
    db.newStatement(
        "SELECT lrn, nrn, issue_date, diagnosis, from_date, to_date, institution, note FROM medical_notice "
        "WHERE amblist_rowid=?"
    );

    db.bind(1, amblistRowid);

    std::vector<MedicalNotice> result;

    while (db.hasRows())
    {
        result.emplace_back();

        auto& n = result.back();

        n.lrn = db.asString(0);
        n.nrn = db.asString(1);
        n.issue_date = db.asString(2);
        n.mkb = db.asString(3);
        n.from_date = db.asString(4);
        n.to_date = db.asString(5);
        n.institution = db.asString(6);
        n.note = db.asString(7);
    }

    return result;
}

std::vector<MedicalNotice> DbMedicalNotice::get(long long amblistRowid)
{
    Db db;
    return get(amblistRowid, db);
}

void DbMedicalNotice::save(const std::vector<MedicalNotice> notices, long long amblistRowid, Db& db)
{
    db.newStatement("DELETE FROM medical_notice WHERE amblist_rowid=?");
    db.bind(1, amblistRowid);
    db.execute();

    for (auto& n : notices)
    {
        db.newStatement(
            "INSERT INTO medical_notice "
            "(amblist_rowid, lrn, nrn, issue_date, diagnosis, from_date, to_date, institution, note) "
            "VALUES (?,?,?,?,?,?,?,?,?) "
        );

        db.bind(1, amblistRowid);
        db.bind(2, n.lrn);
        db.bind(3, n.nrn);
        db.bind(4, n.issue_date.to8601());
        db.bind(5, n.mkb.code());
        db.bind(6, n.from_date.to8601());
        db.bind(7, n.to_date.to8601());
        db.bind(8, n.institution);
        db.bind(9, n.note);

        db.execute();
    }
}

void DbMedicalNotice::save(const std::vector<MedicalNotice> notices, long long amblistRowid)
{
    Db db;
    save(notices, amblistRowid, db);
}
