#include "DbLogin.h"
#include <QDebug>
#include "Model/Parser/Parser.h"
#include "View/ModalDialogBuilder.h"
std::vector<PracticePair> DbLogin::getPracticeList(const std::string& doctorLPK)
{
    openConnection();

    std::string query = 
        "SELECT practice.rzi, practice.name "
        "FROM practice LEFT JOIN practice_doctor ON "
        "practice.rzi = practice_doctor.practice_rzi "
        "WHERE practice_doctor.doctor_lpk = '" + doctorLPK + "' "
        "ORDER BY name ASC";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    std::vector<PracticePair> practiceList;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        practiceList.emplace_back(
            PracticePair{
                std::string{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))},
                std::string{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))}
            }
        );
    }

    sqlite3_finalize(stmt);

    closeConnection();
    
    return practiceList;

}

Practice DbLogin::getPractice(const std::string rziCode)
{
    openConnection();

    std::string query = "SELECT rzi, name, bulstat, firm_address, practice_address, legal_entity, vat, nzok_contract, priceList " 
                        "FROM practice WHERE rzi = '" + rziCode + "'";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    Practice practice;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        practice.rziCode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        practice.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        practice.bulstat = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        practice.firm_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        practice.practice_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        practice.legal_entity = sqlite3_column_int(stmt, 5);
        practice.vat = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        practice.nzok_contract = Parser::parseContract(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
        practice.priceList = Parser::getPriceList(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)));

    }

    sqlite3_finalize(stmt);


    closeConnection();

    return practice;
}



void DbLogin::updatePractice(const Practice& practice, const std::string& currentRZI)
{
    openConnection();

    std::string query = "UPDATE practice SET "
        "rzi = '" + practice.rziCode + "', "
        "name = '" + practice.name + "', "
        "bulstat = '" + practice.bulstat + "', "
        "firm_address = '" + practice.firm_address + "', "
        "practice_address = '" + practice.practice_address + "', "
        "legal_entity = '" + std::to_string(practice.legal_entity) + "', "
        "vat = '" + practice.vat + "', "
        "nzok_contract = '" + Parser::write(practice.nzok_contract) + "' "
        "WHERE rzi = '" + currentRZI + "' ";

    qDebug() << QString::fromStdString(query);

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {

    }

    closeConnection();
}

std::optional<Doctor> DbLogin::getDoctor(const std::string& lpk, const std::string& pass)
{
    auto result{ std::optional<Doctor>{} };

    openConnection();

    std::string query =
        "SELECT fname, mname, lname, egn, spec, several_rhif FROM doctor "
        "WHERE lpk = '" + lpk + "' "
        "AND pass = '" + pass + "' ";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        Doctor doctor;

        doctor.LPK = lpk;
        doctor.fname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        doctor.mname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        doctor.lname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        doctor.egn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        doctor.specialty = sqlite3_column_int(stmt, 4);
        doctor.severalRHIF = sqlite3_column_int(stmt, 5);
   
        doctor.pass = pass;

        result = doctor;
    }

    sqlite3_finalize(stmt);

    closeConnection();

    return result;
}

std::optional<Doctor> DbLogin::getDoctor(const std::string& lpk)
{
    auto result{ std::optional<Doctor>{} };

    openConnection();

    std::string query =
        "SELECT fname, mname, lname, egn, spec, several_rhif, pass FROM doctor "
        "WHERE lpk = '" + lpk + "'";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        Doctor doctor;

        doctor.LPK = lpk;
        doctor.fname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        doctor.mname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        doctor.lname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        doctor.egn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        doctor.specialty = sqlite3_column_int(stmt, 4);
        doctor.severalRHIF = sqlite3_column_int(stmt, 5);
        doctor.pass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

        result = doctor;
    }

    sqlite3_finalize(stmt);

    closeConnection();

    return result;
}

std::unordered_map<std::string, std::string> DbLogin::getDoctorNames()
{
    std::unordered_map <std::string, std::string> doctorNames;

    openConnection();

    std::string query = "SELECT lpk, fname, lname FROM doctor";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
        doctorNames[std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)))]
                = doctorPrefix + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))
                     + " " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));

    sqlite3_finalize(stmt);

    closeConnection();

    return doctorNames;

}

std::vector<PracticeDoctor> DbLogin::getDoctors(const std::string& practiceRZI)
{
    std::vector<PracticeDoctor> result;

    openConnection();

    std::string query = "SELECT practice_doctor.doctor_lpk, doctor.fname, doctor.lname, practice_doctor.admin "
        "FROM practice_doctor LEFT JOIN doctor ON practice_doctor.doctor_lpk = doctor.lpk "
        "WHERE practice_doctor.practice_rzi = '" + practiceRZI + "'";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        result.emplace_back(
            PracticeDoctor{
                std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))),
                doctorPrefix + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))
                     + " " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))),
                static_cast<bool>(sqlite3_column_int(stmt, 3))
            }
        );
    }

    sqlite3_finalize(stmt);

    closeConnection();

    return result;
}

void DbLogin::setDoctorsPracticeList(std::vector<PracticeDoctor> doctors, const std::string& practiceRZI)
{
    openConnection();

    std::string query = "DELETE FROM practice_doctor "
        "WHERE practice_rzi = '" + practiceRZI + "'";

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);


    for (auto& doc : doctors)
    {
        query = "INSERT INTO practice_doctor "
            "VALUES ('" 
            + practiceRZI + "', '" 
            + doc.lpk + "', " 
            + std::to_string(doc.admin) + ")";

        rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    }

    closeConnection();
}

void DbLogin::updateDoctor(const Doctor& doctor, std::string& currentLPK)
{

    openConnection();

    std::string query = "UPDATE doctor SET "
        "lpk = '" + doctor.LPK + "', "
        "spec = " + std::to_string(doctor.specialty) + ", "
        "fname = '" + doctor.fname + "', "
        "mname = '" + doctor.mname + "', "
        "lname = '" + doctor.lname + "', "
        "pass = '" + doctor.pass + "', "
        "egn = '" + doctor.egn + "', "
        "several_rhif = " + std::to_string(doctor.severalRHIF) + " "
        "WHERE lpk = '" + currentLPK + "' ";


   

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {

    }

    closeConnection();
}

void DbLogin::updatePriceList(const std::vector<ProcedureTemplate>& priceList, const std::string& rziCode)
{
    openConnection();

    std::string query = "UPDATE practice SET "
        "priceList = '" + Parser::write(priceList) + "' "
        "WHERE rzi = '" + rziCode + "'";

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    if (rc != SQLITE_OK) {

    }

    closeConnection();


}

void DbLogin::insertDoctor(const Doctor& doctor)
{
    openConnection();

    std::string query = "INSERT INTO doctor (lpk, spec, fname, mname, lname, pass, egn, several_rhif) "
        "VALUES('"
        + doctor.LPK + "', "
        + std::to_string(doctor.specialty) + ", "
        "'" + doctor.fname + "', "
        "'" + doctor.mname + "', "
        "'" + doctor.lname + "', "
        "'" + doctor.pass + "', "
        "'" + doctor.egn + "', "
        + std::to_string(doctor.severalRHIF) + ")";



    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {

    }

    closeConnection();
}

bool DbLogin::getAdminPremission(const std::string& lpk, const std::string& rzi)
{
    openConnection();

    bool admin{ false };

    std::string query = "SELECT admin "
        "FROM practice_doctor " 
        "WHERE doctor_lpk ='" + lpk + "' "
        "AND practice_rzi = '" + rzi + "'";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        admin = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    closeConnection();

    return admin;
}
