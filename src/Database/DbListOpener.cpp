#include "DbListOpener.h"
#include "Model/User/UserManager.h"
#include "Database.h"

std::vector<PatientRow> DbListOpener::getPatientRows()
{
    std::vector<PatientRow> rows;
    rows.reserve(50);

    std::string query =
        "SELECT rowid, id, fname, mname, lname , phone FROM patient ORDER BY id ASC";

    for (Db db(query);db.hasRows();)
    {
       rows.emplace_back(PatientRow{});
       
       rows.back().rowID = db.asRowId(0);
       rows.back().patientRowId = db.asRowId(0);

       rows.back().patientId = db.asString(1);

       rows.back().name = db.asString(2) + " " +
                          db.asString(3) + " " +
                          db.asString(4);

       rows.back().phone = db.asString(5);
    }

    return rows;
}

std::vector<AmbRow> DbListOpener::getAmbRows(const Date& from, const Date& to)
{

    std::vector<AmbRow> rows;
    rows.reserve(50);

    std::string query =
        "SELECT amblist.id, amblist.num, sum(procedure.nzok) > 0, " 
        "amblist.day, amblist.month, amblist.year, "
        "patient.rowid, patient.id, patient.fname, patient.mname, patient.lname, patient.phone "

        "FROM amblist JOIN patient ON amblist.patient_rowid = patient.rowid "
        "LEFT JOIN procedure on amblist.id = procedure.amblist_id "
        "GROUP BY amblist.id "

        "HAVING (amblist.year, amblist.month, amblist.day) "
        "BETWEEN (" + std::to_string(from.year) + ", " + std::to_string(from.month) + ", " + std::to_string(from.day) + ") "
        "AND (" + std::to_string(to.year) + ", " + std::to_string(to.month) + ", " + std::to_string(to.day) + ") "
        "AND amblist.lpk = '" + UserManager::currentUser().doctor.LPK + "' "
        "AND amblist.rzi = '" + UserManager::currentUser().practice.rziCode + "' "
        "ORDER BY amblist.year ASC, amblist.month ASC, amblist.day ASC, amblist.num ASC ";

    for (Db db(query); db.hasRows();)
    {
        rows.emplace_back(AmbRow{});

        auto& row = rows.back();

        row.rowID = db.asRowId(0);
        row.ambNumber = db.asInt(1);
        row.nzok = bool(db.asInt(2));
        row.date = Date{ 
            db.asInt(3),
            db.asInt(4),
            db.asInt(5)
        };
        row.patientRowId = db.asRowId(6);
        row.patientId = db.asString(7);
        row.patientName = db.asString(8) + " " +
                          db.asString(9) + " " +
                          db.asString(10);

        row.patientPhone = db.asString(11);
    }
        

    return rows;
}

std::vector<PerioRow> DbListOpener::getPerioRows(const Date& from, const Date& to)
{
    std::vector<PerioRow> rows;
    rows.reserve(50);

    std::string query =
        "SELECT periostatus.id, periostatus.day, periostatus.month, periostatus.year, patient.rowid, patient.id, patient.fname, patient.mname, patient.lname, patient.phone "
        "FROM periostatus INNER JOIN patient ON periostatus.patient_rowid = patient.rowid "
        "WHERE (periostatus.year, periostatus.month, periostatus.day) "
        "BETWEEN (" + std::to_string(from.year) + ", " + std::to_string(from.month) + ", " + std::to_string(from.day) + ") "
        "AND (" + std::to_string(to.year) + ", " + std::to_string(to.month) + ", " + std::to_string(to.day) + ") "
        "ORDER BY periostatus.year ASC, periostatus.month ASC, periostatus.day ASC ";

    for (Db db(query); db.hasRows();)
    {

        rows.emplace_back(PerioRow{});

        auto& row = rows.back();

        row.rowID = db.asRowId(0);
        row.date = Date{
            db.asInt(1),
            db.asInt(2),
            db.asInt(3)
        };
        row.patientRowId = db.asRowId(4);
        row.patientId = db.asString(5);
        row.patientName = db.asString(6)+ " " + db.asString(7) + " " + db.asString(8);
        row.patientPhone = db.asString(9);

    }

    return rows;
}

#include "Model/Financial/Recipient.h"


std::vector<FinancialRow> DbListOpener::getFinancialRows(const Date& from, const Date& to)
{
    std::vector<FinancialRow> rows;

    if (UserManager::currentUser().practice.rziCode == "") return rows;

    Recipient nzokRecipient(std::stoi(UserManager::currentUser().practice.RHIF()));


    std::string query =
        "SELECT id, num, month_notif > 0, "
        "day, month, year, "
        "recipient_id, recipient_name, recipient_phone "
        "FROM financial "
        "WHERE (year, month, day) "
        "BETWEEN (" + std::to_string(from.year) + ", " + std::to_string(from.month) + ", " + std::to_string(from.day) + ") "
        "AND (" + std::to_string(to.year) + ", " + std::to_string(to.month) + ", " + std::to_string(to.day) + ") "
        "AND practice_rzi = '" + UserManager::currentUser().practice.rziCode + "' "
        "ORDER BY year ASC, month ASC, day ASC ";

    for (Db db(query); db.hasRows();)
    {
        rows.emplace_back(FinancialRow{});

        auto& row = rows.back();

        row.rowID = db.asRowId(0);
        
        row.number = db.asInt(1);
        row.nzok = db.asInt(2);
        row.date = Date(db.asInt(3), db.asInt(4), db.asInt(5));

        if (row.nzok) {
            row.recipientId = nzokRecipient.bulstat;
            row.recipientName = nzokRecipient.name;
            row.recipientPhone = "";

            continue;
        }

        row.recipientId = db.asString(6);
        row.recipientName = db.asString(7);
        row.recipientPhone = db.asString(8);
       
    }

    return rows;

}


void DbListOpener::deleteRecord(const std::string& tableName, long long rowid)
{
    Db::crudQuery("DELETE FROM " + tableName + " WHERE id = " + std::to_string(rowid));
}
