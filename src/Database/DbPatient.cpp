#include "DbPatient.h"
#include "Database.h"
#include "qdebug.h"

long long DbPatient::insert(const Patient& patient)
{
    Db db(
        "INSERT INTO patient "
        "(type, id, birth, sex, fname, mname, lname, "
        "ekatte, address, hirbno, phone, country, institution_num, ehic_num, date_valid) "
        "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
    );

    db.bind(1, patient.type);
    db.bind(2, patient.id);
    db.bind(3, patient.birth.to8601());
    db.bind(4, patient.sex);
    db.bind(5, patient.FirstName);
    db.bind(6, patient.MiddleName);
    db.bind(7, patient.LastName);
    db.bind(8, patient.city.getIdxAsInt());
    db.bind(9, patient.address);
    db.bind(10, patient.HIRBNo);
    db.bind(11, patient.phone);

    if (patient.foreigner)
    {
        db.bind(12, patient.foreigner->country.getCode());
        db.bind(13, patient.foreigner->institution);
        db.bind(14, patient.foreigner->ehic);
        db.bind(15, patient.foreigner->date_valid.to8601());
    }

    if (db.execute()) return db.lastInsertedRowID();

    return 0;

}

bool DbPatient::update(const Patient& patient)
{
    Db db(

        "UPDATE patient SET "
        "type=?,"
        "birth=?,"
        "sex=?,"
        "fname=?,"
        "mname=?,"
        "lname=?,"
        "ekatte=?,"
        "address=?,"
        "hirbno=?,"
        "phone=?,"
        "country=?,"
        "institution_num=?,"
        "ehic_num=?,"
        "date_valid=? "
        "WHERE rowid=?"
    );

    db.bind(1, patient.type);
    db.bind(2, patient.birth.to8601());
    db.bind(3, patient.sex);
    db.bind(4, patient.FirstName);
    db.bind(5, patient.MiddleName);
    db.bind(6, patient.LastName);
    db.bind(7, patient.city.getIdxAsInt());
    db.bind(8, patient.address);
    db.bind(9, patient.HIRBNo);
    db.bind(10, patient.phone);
    

    if (patient.foreigner)
    {
        db.bind(11, patient.foreigner->country.getCode());
        db.bind(12, patient.foreigner->institution);
        db.bind(13, patient.foreigner->ehic);
        db.bind(14, patient.foreigner->date_valid.to8601());
    }
    
    db.bind(15, patient.rowid);

    return db.execute();

}

Patient DbPatient::get(std::string patientID, int type)
{
    std::string query = "SELECT rowid, type, id, birth, sex, fname, mname, lname, ekatte, address, hirbno, phone, country, institution_num, ehic_num, date_valid "
        "FROM patient WHERE id = '" + patientID + "' "
        "AND type = " + std::to_string(type);

    Db db(query);

    Patient patient;

    while (db.hasRows())
    {
        patient.rowid = db.asRowId(0),
        patient.type = Patient::Type(db.asInt(1));
        patient.id = db.asString(2);
        patient.birth = Date(db.asString(3));
        patient.sex = Patient::Sex(db.asInt(4));
        patient.FirstName = db.asString(5);
        patient.MiddleName = db.asString(6);
        patient.LastName = db.asString(7);
        patient.city = Ekatte(db.asInt(8));
        patient.address = db.asString(9);
        patient.HIRBNo = db.asString(10);
        patient.phone = db.asString(11);

        if (patient.type == Patient::EU)
        {
            patient.foreigner = Foreigner{
                .country = Country{db.asString(12)},
                .institution = db.asString(13),
                .ehic = db.asString(14),
                .date_valid = db.asString(15)
            };
        }
    }

    patient.teethNotes = getPresentNotes(patient.rowid);
    patient.medStats = getMedicalStatuses(patient.rowid, db);

    return patient;
}

Patient DbPatient::get(long long rowid)
{
    Db db("SELECT rowid, type, id, birth, sex, fname, mname, lname, ekatte, address, hirbno, phone, country, institution_num, ehic_num, date_valid "
        "FROM patient WHERE rowid = " + std::to_string(rowid)
    );

    Patient patient;

    while (db.hasRows())
    {
        patient.rowid = db.asRowId(0),
        patient.type = Patient::Type(db.asInt(1));
        patient.id = db.asString(2);
        patient.birth = Date(db.asString(3));
        patient.sex = Patient::Sex(db.asInt(4));
        patient.FirstName = db.asString(5);
        patient.MiddleName = db.asString(6);
        patient.LastName = db.asString(7);
        patient.city = db.asInt(8);
        patient.address = db.asString(9);
        patient.HIRBNo = db.asString(10);
        patient.phone = db.asString(11);

        if (patient.type == Patient::EU)
        {
            patient.foreigner = Foreigner{
                .country = Country{db.asString(12)},
                .institution = db.asString(13),
                .ehic = db.asString(14),
                .date_valid = db.asString(15)
            };
        }
    }

    patient.teethNotes = getPresentNotes(patient.rowid);
    patient.medStats = getMedicalStatuses(rowid, db);

    return patient;
}


enum MedStatusType { Allergy, CurrentDiseases, PastDiseases };

bool DbPatient::updateMedStats(long long patientRowId, const MedicalStatuses& s)
{

    Db db("DELETE FROM medical_status WHERE patient_rowid=?");

    db.bind(1, patientRowId);

    db.execute();

    auto lambda = [&](const std::vector<MedicalStatus>& list, MedStatusType t) {

        for (auto& status : list)
        {

            db.newStatement("INSERT INTO medical_status (patient_rowid, nrn, data, type) VALUES (?,?,?,?)");

            db.bind(1, patientRowId);
            db.bind(2, status.nrn);
            db.bind(3, status.data);
            db.bind(4, t);

            db.execute();
        }
    };

    lambda(s.allergies, Allergy);
    lambda(s.condition, CurrentDiseases);
    lambda(s.history, PastDiseases);

    return true;

}


bool DbPatient::updateMedStatus(long long patientRowId, const MedicalStatuses& s, Db& db)
{
    db.newStatement("DELETE FROM medical_status WHERE patient_rowid=?");

    db.bind(1, patientRowId);

    db.execute();

    auto lambda = [&](const std::vector<MedicalStatus>& list, MedStatusType t) {

        for (auto& status : list)
        {

            db.newStatement("INSERT INTO medical_status (patient_rowid, nrn, data, type) VALUES (?,?,?,?)");

            db.bind(1, patientRowId);
            db.bind(2, status.nrn);
            db.bind(3, status.data);
            db.bind(4, t);

            db.execute();
        }
    };

    lambda(s.allergies, Allergy);
    lambda(s.condition, CurrentDiseases);
    lambda(s.history, PastDiseases);

    return true;
}

MedicalStatuses DbPatient::getMedicalStatuses(long long patientRowId)
{
    MedicalStatuses result;

    Db db("SELECT type, nrn, data FROM medical_status WHERE patient_rowid=?");
    db.bind(1, patientRowId);

    while (db.hasRows())
    {
        std::vector<MedicalStatus>* stat;

        switch (db.asInt(0))
        {
            case Allergy: stat = &result.allergies; break;
            case CurrentDiseases: stat = &result.condition; break;
            case PastDiseases: stat = &result.history; break;
            default: continue;
        }

        stat->push_back(MedicalStatus{ .nrn = db.asString(1), .data = db.asString(2) });
    }

    return result;

}

MedicalStatuses DbPatient::getMedicalStatuses(long long patientRowId, Db& db)
{
    MedicalStatuses result;

    db.newStatement("SELECT type, nrn, data FROM medical_status WHERE patient_rowid=?");
    db.bind(1, patientRowId);

    while (db.hasRows())
    {
        std::vector<MedicalStatus>* stat;

        switch (db.asInt(0))
        {
            case Allergy: stat = &result.allergies; break;
            case CurrentDiseases: stat = &result.condition; break;
            case PastDiseases: stat = &result.history; break;
            default: continue;
        }

        stat->push_back(MedicalStatus{ .nrn = db.asString(1), .data = db.asString(2) });
    }

    return result;
}

TeethNotes DbPatient::getPresentNotes(long long patientRowId)
{
    Db db("SELECT tooth, text FROM note WHERE patient_rowid=?");

    db.bind(1, patientRowId);

    TeethNotes notes{};

    while (db.hasRows())
    {
        notes[db.asInt(0)] = db.asString(1);
    }

    return notes;
}
