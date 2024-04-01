#include "DbBrowser.h"
#include "Model/FreeFunctions.h"
#include "Model/User.h"
#include "Database.h"
#include "DbProcedure.h"
#include "DbProcedure.h"
#include "DbPrescription.h"

std::pair<std::vector<RowInstance>, PlainTable> getPatientRows()
{
    std::vector<RowInstance> rows;
    PlainTable tableView;
    
    rows.reserve(50);

    tableView.addColumn({"ЕГН/ЛНЧ/ССН",150,PlainColumn::Center});
    tableView.addColumn({"Име на пациента",250});
    tableView.addColumn({"Телефон",120});

    std::string query =
        "SELECT rowid, id, fname, mname, lname , phone,  (strftime('%m-%d', patient.birth) = strftime('%m-%d',date('now', 'localtime'))) AS bday FROM patient ORDER BY bday DESC, id ASC";

    for (Db db(query);db.hasRows();)
    {
       rows.emplace_back(TabType::PatientSummary);
       
       rows.back().rowID = db.asRowId(0);
       rows.back().patientRowId = db.asRowId(0);

       tableView.addCell(0, { .data = db.asString(1) });

       tableView.addCell(1, { 
           .data = db.asString(2) + " " +
           db.asString(3) + " " +
           db.asString(4),
           .icon = db.asBool(6) ? 
                CommonIcon::BDAY
                :
                CommonIcon::NOICON
       });

       tableView.addCell(2, { 
           .data = db.asString(5) 
       });

    }

    return std::make_pair(rows, tableView);
}

std::pair<std::vector<RowInstance>, PlainTable> getAmbRows(const Date& from, const Date& to)
{

    std::vector<RowInstance> rows;
    PlainTable tableView;

    tableView.addColumn({"Дата",120,PlainColumn::Right});
    tableView.addColumn({"Амб №/НРН",135,PlainColumn::Center});
    tableView.addColumn({"ЕГН/ЛНЧ/ССН",120,PlainColumn::Center});
    tableView.addColumn({"Име на пациента",240});
    tableView.addColumn({"Телефон",120});


    rows.reserve(50);

    std::string query =
        "SELECT "
        "amblist.rowid, "
        "(amblist.nrn != '' AND amblist.nrn IS NOT NULL) AS nhis, "
        "(procedure.financing_source = 2 OR referral.rowid NOT NULL) as nhif, "
        "amblist.nrn, amblist.num, " 
        "amblist.date, "
        "patient.rowid, patient.id, patient.fname, patient.mname, patient.lname, patient.phone, "
        "(strftime('%m-%d', patient.birth) = strftime('%m-%d',date('now', 'localtime'))) AS bday, "
        "amblist.his_updated "
        "FROM amblist "
        "JOIN patient ON amblist.patient_rowid = patient.rowid "
        "LEFT JOIN procedure ON amblist.rowid = procedure.amblist_rowid "
        "LEFT JOIN referral ON amblist.rowid = referral.amblist_rowid "
        "WHERE strftime('%Y-%m-%d', amblist.date) BETWEEN '" + from.to8601() + "' AND '" + to.to8601() + "' "
        "AND amblist.lpk = '" + User::doctor().LPK + "' "
        "AND amblist.rzi = '" + User::practice().rziCode + "' "
        "GROUP BY amblist.rowid "
        "ORDER BY strftime('%Y %m %d', amblist.date) ASC, amblist.num ASC ";

    Db db(query);

    while (db.hasRows())
    {
        rows.emplace_back(TabType::AmbList);

        auto& row = rows.back();
        row.rowID = db.asRowId(0);
        row.patientRowId = db.asRowId(6);

        //Date
        tableView.addCell(0, {
            .data = Date(db.asString(5)).toBgStandard(),
            .icon = db.asBool(2) ? CommonIcon::NHIF : CommonIcon::NOICON
        });
        
        bool his = db.asBool(1);
        bool his_updated = db.asBool(13);

        CommonIcon::Type his_icon = CommonIcon::NOICON;
        if (his) {
            his_icon = his_updated ? CommonIcon::HIS : CommonIcon::HISGRAY;
        }
        //Number
        tableView.addCell(1, {
            .data = his ? db.asString(3) : FreeFn::leadZeroes(db.asInt(4), 6),
            .icon = his_icon
        });

        //ID
        tableView.addCell(2, { .data = db.asString(7) });

        //Name
        tableView.addCell(3, {
             .data = db.asString(8) + " " +
                     db.asString(9) + " " +
                     db.asString(10),
             .icon = db.asBool(12) ?
                    CommonIcon::BDAY
                    :
                    CommonIcon::NOICON
        });

        //Phone
        tableView.addCell(4, { .data = db.asString(11) });
    }
        
    return std::make_pair(rows, tableView);
}

std::pair<std::vector<RowInstance>, PlainTable> getPerioRows(const Date& from, const Date& to)
{
    std::vector<RowInstance> rows;
    PlainTable tableView;
    rows.reserve(50);

    tableView.addColumn({"Дата",120,PlainColumn::Center});
    tableView.addColumn({"ЕГН/ЛНЧ/ССН",150,PlainColumn::Center});
    tableView.addColumn({"Име на пациента",250,});
    tableView.addColumn({"Телефон",120,});

    std::string query =
        "SELECT periostatus.rowid, periostatus.date, patient.rowid, patient.id, patient.fname, patient.mname, patient.lname, patient.phone, "
        "(strftime('%m-%d', patient.birth) = strftime('%m-%d',date('now', 'localtime'))) AS bday "
        "FROM periostatus INNER JOIN patient ON periostatus.patient_rowid = patient.rowid "

        "WHERE "
        "periostatus.date BETWEEN '" + from.to8601() + "' AND '" + to.to8601() + "' "
        "AND lpk = '" + User::doctor().LPK + "' "
        "AND rzi = '" + User::practice().rziCode + "' "
        "ORDER BY periostatus.date ASC ";

    for (Db db(query); db.hasRows();)
    {
        rows.emplace_back(TabType::PerioStatus);

        auto& row = rows.back();

        row.rowID = db.asRowId(0);
        row.patientRowId = db.asRowId(2);

        //Date
        tableView.addCell(0, {.data = Date(db.asString(1)).toBgStandard()});

        tableView.addCell(1, { .data = db.asString(3) });

        tableView.addCell(2, {
             .data = db.asString(4) + " " +
                     db.asString(5) + " " +
                     db.asString(6),
             .icon = db.asBool(8) ?
                    CommonIcon::BDAY
                    :
                    CommonIcon::NOICON
        });

        tableView.addCell(3, { .data = db.asString(7) });
    }

    return std::make_pair(rows, tableView);
}

#include "Model/Financial/Recipient.h"


std::pair<std::vector<RowInstance>, PlainTable> getFinancialRows(const Date& from, const Date& to)
{
    std::vector<RowInstance> rows;
    PlainTable tableView;

    if (User::practice().rziCode == "") return std::make_pair(rows, tableView);

    Recipient nzokRecipient(User::practice());

    tableView.addColumn({"Дата", 120, PlainColumn::Right});
    tableView.addColumn({"Документ №", 110, PlainColumn::Center});
    tableView.addColumn({"ЕГН/ЛНЧ/ЕИК", 100, PlainColumn::Center});
    tableView.addColumn({"Име на получателя", 250 });
    tableView.addColumn({"Телефон", 100 });

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
        rows.emplace_back(TabType::Financial);

        auto& row = rows.back();

        row.rowID = db.asRowId(0);
        
        bool nhif = db.asBool(2);

        //Date
        tableView.addCell(0, {
            .data = Date(db.asString(3)).toBgStandard(),
            .icon = nhif ? CommonIcon::NHIF : CommonIcon::NOICON
            });

        //Number
        tableView.addCell(1, {.data = FreeFn::leadZeroes(db.asInt(1), 10)});
        
        tableView.addCell(2, { .data = nhif ? nzokRecipient.identifier : db.asString(4) });
        tableView.addCell(3, { .data = nhif ? nzokRecipient.name : db.asString(5) });
        tableView.addCell(4, { .data = nhif ? nzokRecipient.phone : db.asString(6) });
      
    }

    return std::make_pair(rows, tableView);

}

std::pair<std::vector<RowInstance>, PlainTable> getPrescriptionRows(const Date& from, const Date& to)
{

    std::vector<RowInstance> rows;
    PlainTable tableView;

    tableView.addColumn({"Дата", 120, PlainColumn::Center});
    tableView.addColumn({"НРН", 135, PlainColumn::Center});
    tableView.addColumn({"ЕГН/ЛНЧ/ССН", 120, PlainColumn::Center});
    tableView.addColumn({"Име на пациента", 240});
    tableView.addColumn({"Телефон", 120});


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
        "patient.phone, "
        "(strftime('%m-%d', patient.birth) = strftime('%m-%d',date('now', 'localtime'))) AS bday "
        "FROM prescription INNER JOIN patient ON prescription.patient_rowid = patient.rowid "
        "WHERE "
        "prescription.date BETWEEN '" + from.to8601() + "' AND '" + to.to8601() + "' "
        "AND prescription.lpk = '" + User::doctor().LPK + "' "
        "AND prescription.rzi = '" + User::practice().rziCode + "' "
        "ORDER BY prescription.date ASC ";

    Db db(query);

    while (db.hasRows())
    {
        rows.emplace_back(TabType::Prescription);

        auto& p = rows.back();

        p.rowID = db.asRowId(0);
        p.patientRowId = db.asRowId(3);

       //Date
       tableView.addCell(0, {
           .data = Date(db.asString(1)).toBgStandard()
       });

       std::string nrn = db.asString(2);
       //NRN
       tableView.addCell(1, {
           .data = nrn,
           .icon = nrn.size() ? CommonIcon::HIS : CommonIcon::NOICON
       });

       //ID
       tableView.addCell(2, { .data = db.asString(4) });

       //NAME
       tableView.addCell(3, {
            .data = db.asString(5) + " " +
                    db.asString(6) + " " +
                    db.asString(7),
            .icon = db.asBool(9) ?
                   CommonIcon::BDAY
                   :
                   CommonIcon::NOICON
           });

       //PHONE
       tableView.addCell(4, { .data = db.asString(8) });
    }

    return std::make_pair(rows, tableView);
}

std::pair<std::vector<RowInstance>, PlainTable> DbBrowser::getPatientDocuments(long long patientRowid)
{
    PlainTable table;

    std::vector<RowInstance> rowidData;

    table.addColumn({"Дата",120,PlainColumn::Right});
    table.addColumn({"Документ",180});
    table.addColumn({"Номер/НРН"});

    Db db;

    db.newStatement(
        "SELECT rowid, 1 as type, date, num, nrn, nhif_spec IS NOT NULL AS nhif, his_updated FROM amblist WHERE patient_rowid=? AND lpk=? AND rzi=?"
        "UNION ALL SELECT rowid, 2 AS type, date, NULL AS num, nrn, NULL as nhif, 1 AS his_updated FROM prescription WHERE patient_rowid=? AND lpk=? AND rzi =?"
        "UNION ALL SELECT rowid, 3 AS type, date, NULL AS num, NULL AS nrn, NULL as nhif, 1 AS his_updated FROM periostatus WHERE patient_rowid=? AND lpk=? AND rzi=?"
        "UNION ALL SELECT financial.rowid, 4 AS type, date, num, NULL AS nrn, NULL as nhif, 1 AS his_updated FROM financial LEFT JOIN patient ON financial.recipient_id = patient.id WHERE patient.rowid=? AND practice_rzi=?"
        "ORDER BY date DESC"
    );

    auto rzi = User::practice().rziCode;
    auto lpk = User::doctor().LPK;

    for (int i = 1; i < 10; i+=3) {
        db.bind(i, patientRowid);
        db.bind(i+1, lpk);
        db.bind(i+2, rzi);
    }

    db.bind(10, patientRowid);
    db.bind(11, rzi);

    while (db.hasRows())
    {
        long long rowid = db.asRowId(0);
        int type = db.asInt(1);
        std::string date = Date(db.asString(2)).toBgStandard();

        //getting nrn
        std::string number = db.asString(4);

        bool nhif = db.asBool(5);

        bool sentToHis = number.size();

        bool his_updated = db.asBool(6);;

        if (number.empty())
        {
            number = std::to_string(db.asInt(3));
        }

        std::string docTypeString;
        CommonIcon::Type docTypeIcon = CommonIcon::NOICON;

        CommonIcon::Type his_icon = CommonIcon::NOICON;

        if (sentToHis) {
            his_icon = his_updated ? CommonIcon::HIS : CommonIcon::HISGRAY;
        }


        switch (type) {
            case 1: 
                docTypeString = "Амбулаторен лист";
                docTypeIcon = CommonIcon::AMBLIST;
                if (!sentToHis) {
                    number = FreeFn::leadZeroes(number, 6); 
                }
                break;
            case 2:
                docTypeString = "Рецепта";
                docTypeIcon = CommonIcon::PRESCR;
                break;
            case 3:
                docTypeString = "Пародонтален статус";
                docTypeIcon = CommonIcon::PERIO;
                number.clear();
                break;
            case 4:
                docTypeString = "Фактура";
                docTypeIcon = CommonIcon::INVOICE;
                number = FreeFn::leadZeroes(number, 10);
                break;
        }


        rowidData.push_back(static_cast<TabType>(type));
        rowidData.back().rowID = rowid;
        //financial
        rowidData.back().patientRowId = type == 4 ? 0 : patientRowid;

        table.addCell(0, { .data = date, .icon = nhif ? CommonIcon::NHIF : CommonIcon::NOICON });
        table.addCell(1, { .data = docTypeString, .icon = docTypeIcon });
        table.addCell(2, { .data = number, .icon = his_icon});
    }

    return std::make_pair(rowidData, table);
};

std::pair<std::vector<RowInstance>, PlainTable> DbBrowser::getData(TabType type, const Date& from, const Date& to)
{
    switch (type)
    {
        case TabType::AmbList: return getAmbRows(from, to);
        case TabType::PerioStatus: return getPerioRows(from, to);
        case TabType::PatientSummary: return getPatientRows();
        case TabType::Financial: return getFinancialRows(from, to);
        case TabType::Prescription: return getPrescriptionRows(from, to);
        default: return {};
    }
}

void DbBrowser::deleteRecord(TabType type, long long rowid)
{
    static const std::map<TabType, std::string_view> tableNames = {
        {TabType::AmbList, "amblist"},
        {TabType::PerioStatus, "periostatus"},
        {TabType::PatientSummary, "patient"},
        {TabType::Financial, "financial"},
        {TabType::Prescription, "prescription"}
    };


    std::string tableName{ tableNames.at(type) };

    Db::crudQuery("DELETE FROM " + tableName + " WHERE rowid = " + std::to_string(rowid));
}
