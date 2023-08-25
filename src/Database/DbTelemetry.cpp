#include "DbTelemetry.h"
#include "Database.h"

DbTelemetry::Data DbTelemetry::getData(const std::string practiceRzi, const std::string doctorLpk)
{
    Data result;

    Db db;

    db.newStatement("SELECT COUNT(*) FROM amblist WHERE amblist.rzi=? AND amblist.lpk=?");
    db.bind(1, practiceRzi);
    db.bind(2, doctorLpk);

    while (db.hasRows()) {
        result.ambCount = db.asInt(0);
    }

    db.newStatement("SELECT COUNT(*) FROM periostatus WHERE periostatus.rzi=? AND periostatus.lpk=?");
    db.bind(1, practiceRzi);
    db.bind(2, doctorLpk);

    while (db.hasRows()) {
        result.perioCount = db.asInt(0);
    }

    db.newStatement("SELECT COUNT(*) FROM prescription WHERE prescription.rzi=? AND prescription.lpk=?");

    db.bind(1, practiceRzi);
    db.bind(2, doctorLpk);

    while (db.hasRows()) {
        result.prescrCount = db.asInt(0);
    }

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
