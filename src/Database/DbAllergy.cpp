#include "DbAllergy.h"
#include "Database/Database.h"

bool DbAllergy::updateAllergies(long long patientRowid, const std::vector<Allergy> allergies)
{
    Db db;

    db.newStatement("DELETE FROM allergy WHERE patient_rowid=?");

    db.bind(1, patientRowid);

    db.execute();

    for (auto& a : allergies)
    {

        db.newStatement(
            "INSERT INTO allergy "
            "(patient_rowid, lrn, nrn, description, type, category, criticality, clinical_status, verification_status, last_occurence) "
            "VALUES (?,?,?,?,?,?,?,?,?,?)"
        );

        db.bind(1, patientRowid);
        db.bind(2, a.lrn);
        db.bind(3, a.nrn);
        db.bind(4, a.description);
        db.bind(5, a.type);
        db.bind(6, a.category);
        db.bind(7, a.criticality);
        db.bind(8, a.clinicalStatus);
        db.bind(9, a.verificationStatus);
        db.bind(10, a.lastOccurence ? a.lastOccurence->to8601() : "");

        if(!db.execute()) return false;
    }

    return true;
}

std::vector<Allergy> DbAllergy::getAllergies(long long patientRowid)
{
    Db db(
        "SELECT FROM allergy lrn, nrn, description, type, category, criticality, clinical_status, verification_status, last_occurence) "
        "WHERE patient_rowid=?"
    );

    db.bind(1, patientRowid);

    std::vector<Allergy> result;

    while (db.hasRows())
    {
        result.emplace_back();
        auto& a = result.back();

        a.lrn = db.asString(0);
        a.nrn = db.asString(1);
        a.description = db.asString(2);
        a.type = static_cast<Allergy::Type>(db.asInt(3));
        a.category = static_cast<Allergy::Category>(db.asInt(4));
        a.criticality = static_cast<Allergy::Criticality>(db.asInt(5));
        a.clinicalStatus = static_cast<Allergy::ClinicalStatus>(db.asInt(6));
        a.verificationStatus = static_cast<Allergy::VerificationStatus>(db.asInt(7));
        
        auto dateStr = db.asString(8);
        
        if (dateStr.size()) {
            a.lastOccurence = Date{ dateStr };
        }
    }

    return result;
}
