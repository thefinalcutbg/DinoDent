#include "DbLogin.h"
#include <QDebug>
#include "Model/Parser/Parser.h"

std::vector<PracticePair> DbLogin::getPracticeList(const std::string& doctorLPK)
{
    openConnection();

    std::string query = 
        "SELECT practice.rzi, practice.name "
        "FROM practice LEFT JOIN practicedoctor ON "
        "practice.rzi = practicedoctor.rzi "
        "WHERE practicedoctor.lpk = '" + doctorLPK + "' "
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
        practice.practice_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        practice.bulstat = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        practice.firm_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        practice.practice_address = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        practice.legal_entity = sqlite3_column_int(stmt, 5);
        practice.vat = static_cast<bool>(sqlite3_column_int(stmt, 6));
        practice.priceList = Parser::getPriceList(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));

    }

    sqlite3_finalize(stmt);

    query = "SELECT contract, date, bank, bic, iban "
            "FROM nzok_contract WHERE rzi = '" + rziCode + "'";


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
        "SELECT lpk, name, spec FROM doctor "
        "WHERE lpk = '" + lpk + "' "
        "AND pass = '" + pass + "' ";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        Doctor doctor;

        doctor.LPK = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        doctor.doctor_name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        doctor.specialty = sqlite3_column_int(stmt, 2);
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

    std::string query = "SELECT lpk, name FROM doctor";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
        doctorNames[std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)))]
                  = u8"д-р " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));

    sqlite3_finalize(stmt);

    closeConnection();

    return doctorNames;

}
