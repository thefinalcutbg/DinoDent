#include "DbLogin.h"
#include <QDebug>

std::vector<Practice> DbLogin::practiceList()
{
    openConnection();

    std::string query = "SELECT rziCode, name, bulstat, contract, contractDate " 
                        "FROM practice ORDER BY name ASC";

    

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    std::vector<Practice> practiceList;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        practiceList.emplace_back(
            Practice{
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))
            }
        );

        if (!practiceList.back().contract.empty())
            practiceList.back().contractDate = Date{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)) };
    }

    closeConnection();

    return practiceList;
}

std::optional<Doctor> DbLogin::getDoctor(const std::string& lpk, const std::string& pass, const std::string& rziCode)
{
    auto result{ std::optional<Doctor>{} };

    openConnection();

    std::string query =
        "SELECT doctor.lpk, doctor.name, doctor.spec FROM"
        " doctor LEFT JOIN practicedoctor ON doctor.lpk = practicedoctor.lpk"
        " WHERE doctor.lpk = '" + lpk + "' "
        " AND pass = '" + pass + "' "
        " AND rziCode = " + rziCode;

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

    closeConnection();

    return result;
}

std::unordered_map<std::string, std::string> DbLogin::getDoctorNames()
{
    std::unordered_map <std::string, std::string> doctorNames;

    openConnection();

    std::string query = "SELECT LPK, name FROM doctor";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
        doctorNames[std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)))]
                  = u8"д-р " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));

    closeConnection();

    return doctorNames;

}
