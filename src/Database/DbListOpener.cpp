#include "DbListOpener.h"
#include "Model/User/UserManager.h"

std::vector<PatientRow> DbListOpener::getPatientRows()
{
    openConnection();

    std::vector<PatientRow> rows;
    rows.reserve(50);

    std::string query =
        "SELECT id, fname, mname, lname , phone FROM patient ORDER BY id ASC";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
       rows.emplace_back(PatientRow{});
       
       rows.back().rowID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
       rows.back().patientId = rows.back().rowID;

       rows.back().name = std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) } + " " +
                          std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) } + " " +
                          std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) };

       rows.back().phone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    }

    sqlite3_finalize(stmt);

    closeConnection();

    return rows;
}

std::vector<AmbRow> DbListOpener::getAmbRows(const Date& from, const Date& to)
{
    openConnection();

    std::vector<AmbRow> rows;
    rows.reserve(50);

    std::string query =
        "SELECT amblist.id, amblist.num, sum(procedure.nzok) > 0, " 
        "amblist.day, amblist.month, amblist.year, "
        "patient.id, patient.fname, patient.mname, patient.lname, patient.phone "

        "FROM amblist JOIN patient ON amblist.patient_id = patient.id "
        "LEFT JOIN procedure on amblist.id = procedure.amblist_id "
        "GROUP BY amblist.id "

        "HAVING (amblist.year, amblist.month, amblist.day) "
        "BETWEEN (" + std::to_string(from.year) + ", " + std::to_string(from.month) + ", " + std::to_string(from.day) + ") "
        "AND (" + std::to_string(to.year) + ", " + std::to_string(to.month) + ", " + std::to_string(to.day) + ") "
        "AND amblist.lpk = '" + UserManager::currentUser().doctor.LPK + "' "
        "AND amblist.rzi = '" + UserManager::currentUser().practice.rziCode + "' "
        "ORDER BY amblist.year ASC, amblist.month ASC, amblist.day ASC, amblist.num ASC ";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        rows.emplace_back(AmbRow{});

        auto& row = rows.back();

        row.rowID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        row.ambNumber = sqlite3_column_int(stmt, 1);
        row.nzok = bool(sqlite3_column_int(stmt, 2));
        row.date = Date{ 
            sqlite3_column_int(stmt, 3),
            sqlite3_column_int(stmt, 4), 
            sqlite3_column_int(stmt, 5) 
        };
        row.patientId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        row.patientName = std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)) } + " " +
                          std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)) } + " " +
                          std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)) };

        row.patientPhone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10));
    }
        

    sqlite3_finalize(stmt);

    closeConnection();

    return rows;
}

std::vector<PerioRow> DbListOpener::getPerioRows(const Date& from, const Date& to)
{

    openConnection();

    std::vector<PerioRow> rows;
    rows.reserve(50);

    std::string query =
        "SELECT periostatus.id, periostatus.day, periostatus.month, periostatus.year, patient.id, patient.fname, patient.mname, patient.lname, patient.phone "
        "FROM periostatus INNER JOIN patient ON periostatus.patient_id = patient.id "
        "WHERE (periostatus.year, periostatus.month, periostatus.day) "
        "BETWEEN (" + std::to_string(from.year) + ", " + std::to_string(from.month) + ", " + std::to_string(from.day) + ") "
        "AND (" + std::to_string(to.year) + ", " + std::to_string(to.month) + ", " + std::to_string(to.day) + ") "
        "ORDER BY periostatus.year ASC, periostatus.month ASC, periostatus.day ASC ";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {

        rows.emplace_back(PerioRow{});

        auto& row = rows.back();

        row.rowID = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        row.date = Date{
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3)
        };
        row.patientId = std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)) };
        row.patientName = std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)) } + " " +
                          std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)) } + " " +
                          std::string{ reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)) };
        row.patientPhone = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));

    }

    sqlite3_finalize(stmt);

    closeConnection();

    return rows;
}


void DbListOpener::deleteRecord(const std::string& tableName, const std::string& id)
{
    openConnection();

    std::string query = "DELETE FROM " + tableName + " WHERE id = '" + id + "' ";

    rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &err);

    closeConnection();
}
