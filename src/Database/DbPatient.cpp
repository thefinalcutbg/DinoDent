#include "DbPatient.h"

DbPatient::DbPatient()
{}


void DbPatient::insert(const Patient& patient)
{
    openConnection();

    std::string query = "INSERT INTO patient VALUES ('"
        + std::to_string(patient.type) + "','"
        + patient.id + "','"
        + Date::toString(patient.birth) + "','"
        + std::to_string(patient.sex) + "','"
        + patient.FirstName + "','"
        + patient.MiddleName + "','"
        + patient.LastName + "','"
        + patient.city + "','"
        + patient.address + "','"
        + patient.HIRBNo + "','"
        + patient.phone + "','"
        + patient.allergies + "','"
        + patient.currentDiseases + "','"
        + patient.pastDiseases +
        +"')";

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {}// << &db;

    closeConnection();
}

void DbPatient::update(const Patient& patient)
{
    openConnection();

    std::string query = "UPDATE patient SET "
        "type = " + std::to_string(patient.type) + ", "
        "birth = '" + Date::toString(patient.birth) + "', "
        "sex = " + std::to_string(patient.sex) + ", "
        "fname = '" + patient.FirstName + "', "
        "mname = '" + patient.MiddleName + "', "
        "lname = '" + patient.LastName + "', "
        "city = '" + patient.city + "', "
        "address = '" + patient.address + "', "
        "hirbno = '" + patient.HIRBNo + "', "
        "phone = '" + patient.phone + "', "
        "allergies = '" + patient.allergies + "', "
        "currentDiseases = '" + patient.currentDiseases + "', "
        "pastDiseases = '" + patient.pastDiseases + "' "
        "WHERE id = '" + patient.id + "'";

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {}// << &db;

    closeConnection();
}

Patient DbPatient::getPatient(std::string patientID)
{
    openConnection();

    std::string query = "SELECT * "
        "FROM patient WHERE id = '" + patientID + "'";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    Patient patient;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        patient.id = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        if (patient.id == "") break;

        patient.type = sqlite3_column_int(stmt, 0);
        patient.birth = Date(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))));
        patient.sex = sqlite3_column_int(stmt, 3);
        patient.FirstName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        patient.MiddleName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
        patient.LastName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)));
        patient.city = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
        patient.address = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));
        patient.HIRBNo = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)));
        patient.phone = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10)));
        patient.allergies = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11)));
        patient.currentDiseases = (reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12)));
        patient.pastDiseases = (reinterpret_cast<const char*>(sqlite3_column_text(stmt, 13)));
    }

    sqlite3_finalize(stmt);

    closeConnection();

    return patient;
}

void DbPatient::updateAllergies(const std::string& patientID, const std::string& allergies, const std::string& current, const std::string& past)
{
    openConnection();

    std::string query = "UPDATE patient SET "
        "allergies = '" + allergies + "', "
        "currentDiseases = '" + current + "', "
        "pastDiseases = '" + past + "' "
        "WHERE id = '" + patientID + "'";

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {}// << &db;

    closeConnection();
}