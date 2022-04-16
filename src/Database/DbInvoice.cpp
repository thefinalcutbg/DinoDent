#include "DbInvoice.h"
#include "Database/Database.h"
#include "Model/User/UserManager.h"
#include "Model/Parser/Parser.h"

long long DbInvoice::insertInvoice(const Invoice& invoice)
{

    bool nzok = invoice.nzokData.has_value();
   

    std::string query;

    if(nzok){
        query = 
            "INSERT INTO financial (practice_rzi, num, type, day, month, year, month_notif, recipient_id, data) "
            "VALUES ("
            "'" + UserManager::currentUser().practice.rziCode + "',"
            + std::to_string(invoice.number) + ","
            + std::to_string(static_cast<int>(invoice.type)) + ","
            + std::to_string(invoice.date.day) + ","
            + std::to_string(invoice.date.month) + ","
            + std::to_string(invoice.date.year) + ","
            + std::to_string(invoice.nzokData->fin_document_month_no) + ","
            "'" + invoice.recipient.bulstat + "' "
            "'" + invoice.nzokData->monthNotifData + "'"
            ")";
    }
    else
    {
        query = "INSERT INTO financial (practice_rzi, num, type, day, month, year, month_notif, data," 
                                " recipient_id, recipient_name, recipient_address, recipient_phone) "
            "VALUES ("
            "'" + UserManager::currentUser().practice.rziCode + "',"
            + std::to_string(invoice.number) + ","
            + std::to_string(static_cast<int>(invoice.type)) + ","
            + std::to_string(invoice.date.day) + ","
            + std::to_string(invoice.date.month) + ","
            + std::to_string(invoice.date.year) + ","
            + "0" + ","
            "'" + Parser::write(invoice) + "',"
            "'" + invoice.recipient.bulstat + "',"
            "'" + invoice.recipient.name + "',"
            "'" + invoice.recipient.address + "',"
            "'" + invoice.recipient.phone + "'"
            ")";
    }


    Db db;

    db.execute(query);

    return db.lastInsertedRowID();


}

void DbInvoice::updateInvoice(const Invoice& invoice)
{

    std::string query =

        "UPDATE financial SET "

        "num = " + std::to_string(invoice.number) + ","
        "type = " + std::to_string(static_cast<int>(invoice.type)) + ","
        "day = " + std::to_string(invoice.date.day) + ","
        "month = " + std::to_string(invoice.date.month) + ","
        "year = " + std::to_string(invoice.date.year) + ","
        "recipient_id = '" + invoice.recipient.bulstat + "', "
        "data = '" + Parser::write(invoice) + "' "

        ;

    if (!invoice.nzokData.has_value()) {


        query += ","
            "recipient_id = '" + invoice.recipient.bulstat + "', "
            "recipient_name = '" + invoice.recipient.name + "', "
            "recipient_phone = '" + invoice.recipient.phone + "', "
            "recipient_address = '" + invoice.recipient.name + "' ";

    }

    query += "WHERE rowid = " + std::to_string(invoice.rowId);

    Db::crudQuery(query);
}

long long DbInvoice::invoiceAlreadyExists(int monthNotifNumber)
{


     std::string query =
         "SELECT rowid FROM financial "
         "WHERE month_notif = " + std::to_string(monthNotifNumber) + " "
         "AND practice_rzi = '" + UserManager::currentUser().practice.rziCode + "' "
         "ORDER BY num DESC LIMIT 1";

     Db db(query);

     while (db.hasRows()) {
         return db.asRowId(0);
     }

     return 0;
}

std::optional<Date> DbInvoice::getMainDocDate(long long invoiceNumber, const std::string& recipient_id)
{
    std::string query{
        "SELECT day, month, year FROM financial WHERE "
        "num = " + std::to_string(invoiceNumber) + " "
        "AND type = 0 "
        "AND recipient_id = '" + recipient_id + "' "
        "AND practice_rzi = '" + UserManager::currentUser().practice.rziCode + "'"
    };

    for (Db db(query); db.hasRows();)
    {
        return Date(db.asInt(0), db.asInt(1), db.asInt(2));
    }

    return {};
}

std::optional<MainDocument> DbInvoice::getMainDocument(const std::string& recipient_id)
{
    std::string query{
    "SELECT num, day, month, year FROM financial WHERE "
    "type = 0 "
    "AND recipient_id = '" + recipient_id + "' "
    "AND practice_rzi = '" + UserManager::currentUser().practice.rziCode + "'"
    "ORDER BY num DESC LIMIT 1"
    };

    for (Db db(query); db.hasRows();)
    {
        return MainDocument{
            db.asLongLong(0),
            Date{
                db.asInt(1),
                db.asInt(2),
                db.asInt(3) }
        };
    }

    return {};
}

#include "Libraries/TinyXML/tinyxml.h"

Invoice DbInvoice::getInvoice(long long rowId)
{
    std::string query = "SELECT num, type, day, month, year, month_notif, data, "
        "recipient_id, recipient_name, recipient_phone, recipient_address "
        "FROM financial "
        "WHERE rowid = " + std::to_string(rowId);

    Db db(query);

    while (db.hasRows()) {
        
        long long invNumber = db.asLongLong(0);
        FinancialDocType type = static_cast<FinancialDocType>(db.asInt(1));
        Date invDate{
            db.asInt(2),
            db.asInt(3),
            db.asInt(4)
        };

        int monthNotif = db.asInt(5);


        //if it's from monthly notification, parse the xml data and return the result:
                    if (monthNotif) {

                        TiXmlDocument doc;
                        doc.Parse(db.asString(6).c_str(), 0, TIXML_ENCODING_UTF8);

                        Invoice result(doc, UserManager::currentUser());

                        result.rowId = rowId;
                        result.number = invNumber;
                        result.date = invDate;

                        return result;
                    }

        //if not, we continue with recipient data, since they are not null for sure

        Invoice inv;

        inv.rowId = rowId;
        inv.number = invNumber;
        inv.date = invDate;
        inv.type = type;

        Parser::parse(db.asString(6), inv);
        inv.recipient.bulstat = db.asString(7);
        inv.recipient.name = db.asString(8);
        inv.recipient.phone = db.asString(9);
        inv.recipient.address = db.asString(10);

  

        return inv;
    }

    return Invoice();
}

int DbInvoice::getNewInvoiceNumber()
{
    int number = 0;

    Db db(
        "SELECT num FROM financial WHERE "
        "practice_rzi = '" + UserManager::currentUser().practice.rziCode + "' "
        "ORDER BY num DESC LIMIT 1"
    );

    while (db.hasRows()) number = db.asInt(0);

    return number + 1;
}

std::unordered_set<int> DbInvoice::getExistingNumbers()
{
    std::unordered_set<int> result;

    Db db(
        "SELECT num FROM financial WHERE "
        "practice_rzi = '" + UserManager::currentUser().practice.rziCode + "' "
    );

    while (db.hasRows()) {
        result.insert(db.asInt(0));
    }

    return result;
}
