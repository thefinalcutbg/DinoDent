#include "DbXML.h"
#include "Model/CityCode.h"
#include "Model/Parser/Parser.h"
#include <QDebug>
std::string leadZeroes(int num, int totalLength)
{
    std::string formated;
    formated.reserve(totalLength);

    std::string number(std::to_string(num));

    for (int i = 0; i < totalLength - number.length(); i++)
        formated += "0";

    formated += number;

    return formated;
}

std::string getAllergiesAndStuff(const std::string& allergiesAndStuff)
{
    if (!allergiesAndStuff.size())
        return u8"Не съобщава";
    return allergiesAndStuff;
}

std::vector<AmbListXML> DbXML::getAmbListXML(Date from, Date to, std::string RZICode, std::string LPK)
{
    openConnection();



    std::string query =
        "SELECT "
        "patient.type,"                 //0
        "patient.id,"                   //1
        "patient.city,"                 //2
        "patient.fname,"                //3
        "patient.mname,"                //4
        "patient.lname,"                //5
        "amblist.num,"                  //6
        "patient.hirbno,"               //7
        "amblist.unfavourable,"         //8
        "patient.allergies,"            //9
        "patient.pastDiseases,"         //10
        "patient.currentDiseases,"      //11
        "amblist.status_json "          //12
        "FROM amblist "
        "LEFT JOIN patient ON amblist.patient_id = patient.id "
        "LEFT JOIN procedure ON amblist.id = procedure.amblist_id "
        "GROUP BY amblist.id "
        "HAVING(amblist.year, amblist.month, amblist.day) "
        "BETWEEN (" + std::to_string(from.year) + ", " + std::to_string(from.month) + ", " + std::to_string(from.day) + ") "
        "AND (" + std::to_string(to.year) + ", " + std::to_string(to.month) + ", " + std::to_string(to.day) + ") "
        "AND sum(procedure.nzok) > 0 "
        "AND amblist.rzi = '" + RZICode + "' "
        "AND amblist.lpk = '" + LPK + "' "
        "ORDER BY amblist.num";

        sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

        std::vector<AmbListXML> report;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        report.emplace_back();
        auto& amb = report.back();

        amb.personType = sqlite3_column_int(stmt, 0);
        amb.personIdentifier = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

        auto city = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        amb.RHIF = CityCode::getCodes(city).first;
        amb.HealthRegion = CityCode::getCodes(city).second;

        amb.personFirstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        amb.personMiddleName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        amb.personLastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        amb.ambulatorySheetNo = leadZeroes(sqlite3_column_int(stmt, 6), 6);
        amb.HIRBNo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        amb.unfavourable_condition = static_cast<bool>(sqlite3_column_int(stmt, 8));
        amb.allergies = getAllergiesAndStuff(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)));
        amb.currentDiseases = getAllergiesAndStuff(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10)));
        amb.pastDiseases = getAllergiesAndStuff(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11)));
        amb.teeth = Parser::getTeethXML(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12)));
    }

    return report;
}
