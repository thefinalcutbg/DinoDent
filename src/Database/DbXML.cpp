#include "DbXML.h"
#include "Model/CityCode.h"

std::vector<AmbListXML> DbXML::getAmbListXML(Date from, Date to, std::string RZICode, std::string LPK)
{
    openConnection();

    

    std::string query =
        "SELECT "
        "patient.type, patient.id, patient.city,"
        "patient.fname, patient.mname, patient.lname,"
        " amblist.num, patient.hirbno, amblist.unfavourable,"
        "patient.allergies,"
        "patient.pastDiseases,"
        "patient.currentDiseases,"
        "amblist.status_json "
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

        amb.type = sqlite3_column_int(stmt, 0);
        amb.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        amb.city = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            
    }

    return report;
}
