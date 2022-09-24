#include "DbListOpener.h"
#include "Model/User.h"
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
        "SELECT amblist.rowid, amblist.num, sum(procedure.nzok) > 0, " 
        "amblist.date, "
        "patient.rowid, patient.id, patient.fname, patient.mname, patient.lname, patient.phone "

        "FROM amblist JOIN patient ON amblist.patient_rowid = patient.rowid "
        "LEFT JOIN procedure on amblist.rowid = procedure.amblist_rowid "
        "GROUP BY amblist.rowid "
        "HAVING strftime('%Y-%m-%d', amblist.date) BETWEEN '" + from.to8601() + "' AND '" + to.to8601() + "' "
        "AND amblist.lpk = '" + User::doctor().LPK + "' "
        "AND amblist.rzi = '" + User::practice().rziCode + "' "
        "ORDER BY strftime('%Y %m %d', amblist.date) ASC, amblist.num ASC ";

    Db db(query);

    while (db.hasRows())
    {
        rows.emplace_back(AmbRow{});

        auto& row = rows.back();
        
        row.rowID = db.asRowId(0);
        row.ambNumber = db.asInt(1);
        
        row.nhif = bool(db.asInt(2));
        row.date = db.asString(3);
        
        row.patientRowId = db.asRowId(4);

        row.patientId = db.asString(5);
        row.patientName = db.asString(6) + " " +
                          db.asString(7) + " " +
                          db.asString(8);

        row.patientPhone = db.asString(9);
    }
        

    return rows;
}

std::vector<PerioRow> DbListOpener::getPerioRows(const Date& from, const Date& to)
{
    std::vector<PerioRow> rows;
    rows.reserve(50);

    std::string query =
        "SELECT periostatus.rowid, periostatus.date, patient.rowid, patient.id, patient.fname, patient.mname, patient.lname, patient.phone "
        "FROM periostatus INNER JOIN patient ON periostatus.patient_rowid = patient.rowid "
        "WHERE "
        "periostatus.date BETWEEN '" + from.to8601() + "' AND '" + to.to8601() + "' "
        "AND lpk = '" + User::doctor().LPK + "' "
        "AND rzi = '" + User::practice().rziCode + "' "
        "ORDER BY periostatus.date ASC ";

    for (Db db(query); db.hasRows();)
    {

        rows.emplace_back(PerioRow{});

        auto& row = rows.back();

        row.rowID = db.asRowId(0);
        row.date = db.asString(1);
        row.patientRowId = db.asRowId(2);
        row.patientId = db.asString(3);
        row.patientName = db.asString(4)+ " " + db.asString(5) + " " + db.asString(6);
        row.patientPhone = db.asString(7);

    }

    return rows;
}

#include "Model/Financial/Recipient.h"


std::vector<FinancialRow> DbListOpener::getFinancialRows(const Date& from, const Date& to)
{
    std::vector<FinancialRow> rows;

    if (User::practice().rziCode == "") return rows;

    Recipient nzokRecipient(std::stoi(User::practice().RHIF()));


    std::string query =
        "SELECT rowid, num, month_notif > 0, "
        "date, "
        "recipient_id, recipient_name, recipient_phone "
        "FROM financial "
        "WHERE "
        "date BETWEEN '" + from.to8601() + "' AND '" + to.to8601() + "' "
        "AND practice_rzi = '" + User::practice().rziCode + "' "
        "ORDER BY date ASC ";

    for (Db db(query); db.hasRows();)
    {
        rows.emplace_back(FinancialRow{});

        auto& row = rows.back();

        row.rowID = db.asRowId(0);
        
        row.number = db.asInt(1);
        row.nhif = db.asInt(2);
        row.date = db.asString(3);

        if (row.nhif) {
            row.recipientId = nzokRecipient.bulstat;
            row.recipientName = nzokRecipient.name;
            row.recipientPhone = "";

            continue;
        }

        row.recipientId = db.asString(4);
        row.recipientName = db.asString(5);
        row.recipientPhone = db.asString(6);
       
    }

    return rows;

}

std::vector<PrescriptionRow> DbListOpener::getPrescriptionRows(const Date& from, const Date& to)
{
    std::vector<PrescriptionRow> rows;
    rows.reserve(50);

    std::string query =
        "SELECT " 
        "prescription.rowid, "
        "prescription.date, "
        "prescription.nrn, "
        "patient.rowid, "
        "patient.id, "
        "patient.fname, "
        "patient.mname, "
        "patient.lname, "
        "patient.phone "
        "FROM prescription INNER JOIN patient ON prescription.patient_rowid = patient.rowid "
        "WHERE "
        "prescription.date BETWEEN '" + from.to8601() + "' AND '" + to.to8601() + "' "
        "AND prescription.lpk = '" + User::doctor().LPK + "' "
        "AND prescription.rzi = '" + User::practice().rziCode + "' "
        "ORDER BY prescription.date ASC ";

    Db db(query);

    while (db.hasRows())
    {
        rows.emplace_back();

        auto& p = rows.back();

       p.rowID = db.asRowId(0);
       p.date = db.asString(1);
       p.nrn = db.asString(2);
       p.patientRowId = db.asRowId(3);
       p.patientId = db.asString(4);
       p.patientName = db.asString(5) + " " + db.asString(6) + " " + db.asString(7);
       p.patientPhone = db.asString(8);
    }

    return rows;
}


void DbListOpener::deleteRecord(TabType type, long long rowid)
{
    static constexpr const char* tableNames[5]{ "amblist", "periostatus", "patient", "financial", "prescription" };

    std::string tableName{ tableNames[static_cast<int>(type)] };

    Db::crudQuery("DELETE FROM " + tableName + " WHERE rowid = " + std::to_string(rowid));
}
