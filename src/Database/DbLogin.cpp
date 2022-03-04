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
