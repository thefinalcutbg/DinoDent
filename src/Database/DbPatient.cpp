#include "DbPatient.h"
#include "Database.h"
#include "Model/CityCode.h"

void DbPatient::insert(const Patient& patient)
{
    Db::crudQuery(

            "INSERT INTO patient VALUES ('"
            + std::to_string(patient.type) + "','"
            + patient.id + "','"
            + patient.birth.toString() + "','"
            + std::to_string(patient.sex) + "','"
            + patient.FirstName + "','"
            + patient.MiddleName + "','"
            + patient.LastName + "',"
            + std::to_string(CityCode::getDbCityIdx(patient.city)) + ",'"
            + patient.address + "','"
            + patient.HIRBNo + "','"
            + patient.phone + "','"
            + patient.allergies + "','"
            + patient.currentDiseases + "','"
            + patient.pastDiseases +
            +"')"
    );


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
        "WHERE id = '" + patient.id + "'"
    );

}

Patient DbPatient::getPatient(std::string patientID)
{
    std::string query = "SELECT * "
        "FROM patient WHERE id = '" + patientID + "'";

    Patient patient;

    for (Db db(query); db.hasRows();)
    {
        patient.id = db.asString(1);
        if (patient.id == "") break;

        patient.type = db.asInt(0);
        patient.birth = Date(db.asString(2));
        patient.sex = db.asInt(3);
        patient.FirstName = db.asString(4);
        patient.MiddleName = db.asString(5);
        patient.LastName = db.asString(6);
        patient.city = CityCode::cityFromIndex(db.asInt(7));
        patient.address = db.asString(8);
        patient.HIRBNo = db.asString(9);
        patient.phone = db.asString(10);
        patient.allergies = db.asString(11);
        patient.currentDiseases = db.asString(12);
        patient.pastDiseases = db.asString(13);
    }

    patient.teethNotes = getPresentNotes(patient.id);

    return patient;
}

void DbPatient::updateAllergies(const std::string& patientID, const std::string& allergies, const std::string& current, const std::string& past)
{
    Db::crudQuery(
        
        "UPDATE patient SET "
        "allergies = '" + allergies + "', "
        "currentDiseases = '" + current + "', "
        "pastDiseases = '" + past + "' "
        "WHERE id = '" + patientID + "'"

    );
}

TeethNotes DbPatient::getPresentNotes(const std::string& patientID)
{

    std::string query = "SELECT tooth, text "
        "FROM note WHERE patient_id = '" + patientID + "'";

    TeethNotes notes{};

    Db db(query);

    while (db.hasRows())
    {
        notes[db.asInt(0)] = db.asString(1);
    }

    return notes;
}
