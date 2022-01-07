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

std::optional<User> DbLogin::getUser(const std::string& lpk, const std::string& pass, const std::string& rziCode)
{
    auto result = std::optional<User>{};

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
        User user;

        user.LPK = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        user.name = u8"д-р " + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        user.specialty = sqlite3_column_int(stmt, 2);
        user.RZI = rziCode;
        result = user;
    }

    closeConnection();

    return result;
}
