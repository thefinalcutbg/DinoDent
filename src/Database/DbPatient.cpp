#include "DbPatient.h"
#include "Database.h"
#include "Model/CityCode.h"

long long DbPatient::insert(const Patient& patient)
{
    Db db;

    bool success = db.execute(

            "INSERT INTO patient "
            "(type, id, birth, sex, fname, mname, lname, " 
            "city, address, hirbno, phone , allergies, pastDiseases, currentDiseases) " 
            "VALUES ("
            + std::to_string(patient.type) + ","
            "'" + patient.id + "',"
            "'" + patient.birth.toString() + "',"
                + std::to_string(patient.sex) + ","
            "'" + patient.FirstName + "',"
            "'" + patient.MiddleName + "',"
            "'" + patient.LastName + "',"
                + std::to_string(CityCode::getDbCityIdx(patient.city)) + ","
            "'" + patient.address + "',"
            "'" + patient.HIRBNo + "',"
            "'" + patient.phone + "',"
            "'" + patient.allergies + "',"
            "'" + patient.currentDiseases + "',"
            "'" + patient.pastDiseases +  "'"
            +")"
    );

    if(success) return db.lastInsertedRowID();

    return 0;
}

void DbPatient::update(const Patient& patient)
{
    Db::crudQuery(

        "UPDATE patient SET "
        "type = " + std::to_string(patient.type) + ", "
        "birth = '" + patient.birth.toString() + "', "
        "sex = " + std::to_string(patient.sex) + ", "
        "fname = '" + patient.FirstName + "', "
        "mname = '" + patient.MiddleName + "', "
        "lname = '" + patient.LastName + "', "
        "city = " + std::to_string(CityCode::getDbCityIdx(patient.city)) + ", "
        "address = '" + patient.address + "', "
        "hirbno = '" + patient.HIRBNo + "', "
        "phone = '" + patient.phone + "', "
        "allergies = '" + patient.allergies + "', "
        "currentDiseases = '" + patient.currentDiseases + "', "
        "pastDiseases = '" + patient.pastDiseases + "' "
        "WHERE rowid = " + std::to_string(patient.rowid)
    );

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
        patient.city = CityCode::cityFromIndex(db.asInt(8));
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
        patient.city = CityCode::cityFromIndex(db.asInt(8));
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

void DbPatient::updateAllergies(long long patientRowId, const std::string& allergies, const std::string& current, const std::string& past)
{
    Db::crudQuery(

        "UPDATE patient SET "
        "allergies = '" + allergies + "', "
        "currentDiseases = '" + current + "', "
        "pastDiseases = '" + past + "' "
        "WHERE rowid = " + std::to_string(patientRowId)

    );
}

TeethNotes DbPatient::getPresentNotes(long long patientRowId)
{

    std::string query = "SELECT tooth, text "
        "FROM note WHERE patient_rowid = " + std::to_string(patientRowId);

    TeethNotes notes{};

    Db db(query);

    while (db.hasRows())
    {
        notes[db.asInt(0)] = db.asString(1);
    }

    return notes;
}
