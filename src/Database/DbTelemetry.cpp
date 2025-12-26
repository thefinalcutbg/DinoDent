#include "DbTelemetry.h"
#include "Database.h"

DbTelemetry::Data DbTelemetry::getData(const std::string practiceRzi, const std::string doctorLpk)
{
    Data result;

    Db db;

    auto dbGetFn = [&](const std::string query) {
        db.newStatement(query);

        db.bind(1, practiceRzi);
        db.bind(2, doctorLpk);

        while (db.hasRows()) {
            return db.asInt(0);
        }

        return 0;
      };

    result.ambCount = dbGetFn("SELECT COUNT(*) FROM amblist WHERE amblist.rzi=? AND amblist.lpk=?");
    result.perioCount = dbGetFn("SELECT COUNT(*) FROM periostatus WHERE periostatus.rzi=? AND periostatus.lpk=?");
    result.prescrCount = dbGetFn("SELECT COUNT(*) FROM prescription WHERE prescription.rzi=? AND prescription.lpk=?");
    result.noticeCount = dbGetFn("SELECT COUNT(*) FROM medical_notice LEFT JOIN amblist ON medical_notice.amblist_rowid = amblist.rowid WHERE amblist.rzi=? AND amblist.lpk=?");
    result.planCount = dbGetFn("SELECT COUNT(*) FROM treatment_plan WHERE amblist.rzi=? AND amblist.lpk=?");
    db.newStatement("SELECT COUNT(*) FROM financial WHERE financial.practice_rzi=?");
    db.bind(1, practiceRzi);

    while (db.hasRows()) {
        result.invoiceCount = db.asInt(0);
    }

    db.newStatement("SELECT COUNT(*) FROM patient");

    while (db.hasRows()) {
        result.patientCount = db.asInt(0);
    }

    return result;
}
