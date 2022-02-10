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

    std::string query = "SELECT rzi, name, bulstat, firm_address, practice_address, legal_entity, vat, priceList " 
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
        practice.vat = static_cast<bool>(sqlite3_column_int(stmt, 6));
        practice.priceList = Parser::getPriceList(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));

    }

    sqlite3_finalize(stmt);

    query = "SELECT contract, date, bank, bic, iban "
            "FROM nzok_contract WHERE practice_rzi = '" + rziCode + "'";


    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        practice.nzok_contract.emplace(

            NzokContract{
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
                Date{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))},
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))
            }

        );
    }


    closeConnection();

    return practice;
}

std::optional<Doctor> DbLogin::getDoctor(const std::string& lpk, const std::string& pass)
{
    auto result{ std::optional<Doctor>{} };

    openConnection();

    std::string query =
        "SELECT fname, mname, lname, egn, spec FROM doctor "
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
        "egn = '" + doctor.egn + "' "
        "WHERE lpk = '" + currentLPK + "' ";


   

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);
    if (rc != SQLITE_OK) {

    }

    closeConnection();
}
