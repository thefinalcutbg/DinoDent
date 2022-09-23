#include "DbPatient.h"
#include "Database.h"
#include "qdebug.h"

long long DbPatient::insert(const Patient& patient)
{
    Db db(
        "INSERT INTO patient "
        "(type, id, birth, sex, fname, mname, lname, "
        "ekatte, address, hirbno, phone , allergies, pastDiseases, currentDiseases) "
        "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)"
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
    db.bind(12, patient.allergies);
    db.bind(13, patient.pastDiseases);
    db.bind(14, patient.currentDiseases);

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
        "allergies=?,"
        "currentDiseases=?,"
        "pastDiseases=? "
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
    db.bind(11, patient.allergies);
    db.bind(12, patient.pastDiseases);
    db.bind(13, patient.currentDiseases);
    db.bind(14, patient.rowid);

    return db.execute();

}

Patient DbPatient::get(std::string patientID, int type)
{
    std::string query = "SELECT * "
        "FROM patient WHERE id = '" + patientID + "' "
        "AND type = " + std::to_string(type);

    Patient patient;

    for (Db db(query); db.hasRows();)
    {
        patient.rowid = db.asRowId(0),
        patient.type = db.asInt(1);
        patient.id = db.asString(2);
        patient.birth = Date(db.asString(3));
        patient.sex = db.asInt(4);
        patient.FirstName = db.asString(5);
        patient.MiddleName = db.asString(6);
        patient.LastName = db.asString(7);
        patient.city = Ekatte(db.asInt(8));
        patient.address = db.asString(9);
        patient.HIRBNo = db.asString(10);
        patient.phone = db.asString(11);
        patient.allergies = db.asString(12);
        patient.currentDiseases = db.asString(13);
        patient.pastDiseases = db.asString(14);
    }

    patient.teethNotes = getPresentNotes(patient.rowid);

    return patient;
}

Patient DbPatient::get(long long rowid)
{
    std::string query = "SELECT * "
        "FROM patient WHERE rowid = " + std::to_string(rowid);

    Patient patient;

    for (Db db(query); db.hasRows();)
    {
        patient.rowid = db.asRowId(0),
        patient.type = db.asInt(1);
        patient.id = db.asString(2);
        patient.birth = Date(db.asString(3));
        patient.sex = db.asInt(4);
        patient.FirstName = db.asString(5);
        patient.MiddleName = db.asString(6);
        patient.LastName = db.asString(7);
        patient.city = db.asInt(8);
        patient.address = db.asString(9);
        patient.HIRBNo = db.asString(10);
        patient.phone = db.asString(11);
        patient.allergies = db.asString(12);
        patient.currentDiseases = db.asString(13);
        patient.pastDiseases = db.asString(14);
    }

    patient.teethNotes = getPresentNotes(patient.rowid);

    return patient;
}

bool DbPatient::updateAllergies(long long patientRowId, const std::string& allergies, const std::string& current, const std::string& past)
{
     Db db(
        "UPDATE patient SET "
        "allergies=?,"
        "currentDiseases=?,"
        "pastDiseases=? "
        "WHERE rowid=?" + std::to_string(patientRowId)
    );

     db.bind(1, allergies); 
     db.bind(2, current); 
     db.bind(3, past); 
     db.bind(4, patientRowId);

     return db.execute();
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
