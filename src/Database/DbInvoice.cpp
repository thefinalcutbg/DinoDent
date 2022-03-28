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
            "INSERT INTO financial (practice_rzi, num, day, month, year, month_notif, data) "
            "VALUES ("
            "'" + UserManager::currentUser().practice.rziCode + "',"
            + std::to_string(invoice.number) + ","
            + std::to_string(invoice.date.day) + ","
            + std::to_string(invoice.date.month) + ","
            + std::to_string(invoice.date.year) + ","
            + std::to_string(invoice.nzokData->fin_document_month_no) + ","
            "'" + invoice.nzokData->monthNotifData + "'"
            ")";
    }
    else
    {
        query = "INSERT INTO financial (practice_rzi, num, day, month, year, month_notif, data," 
                                " recipient_id, recipient_name, recipient_address, recipient_phone) "
            "VALUES ("
            "'" + UserManager::currentUser().practice.rziCode + "',"
            + std::to_string(invoice.number) + ","
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
        "day = " + std::to_string(invoice.date.day) + ","
        "month = " + std::to_string(invoice.date.month) + ","
        "year = " + std::to_string(invoice.date.year) + ","
        "data = '" + Parser::write(invoice) + "' "
        
        ;

    if (!invoice.nzokData.has_value()) {


        query += ","
            "recipient_id = '" + invoice.recipient.bulstat + "', "
            "recipient_name = '" + invoice.recipient.name + "', "
            "recipient_phone = '" + invoice.recipient.phone + "', "
            "recipient_address = '" + invoice.recipient.name + "' ";

    }

    query += "WHERE id = " + invoice.rowId;

    Db::crudQuery(query);
}

std::optional<NzokFinancialDetails> DbInvoice::getDetailsIfAlreadyExist(int monthNotifNumber)
{


     std::string query =
         "SELECT id, num, day, month, year FROM financial "
         "WHERE month_notif = " + std::to_string(monthNotifNumber) + " "
         "AND practice_rzi = '" + UserManager::currentUser().practice.rziCode + "'";

     Db db(query);

     while (db.hasRows()) {

         return NzokFinancialDetails{

                    db.asRowId(0), 
                    db.asInt(1), 
                    Date {
                        db.asInt(2), 
                        db.asInt(3), 
                        db.asInt(4)
                    } 
         };
     }

     return {};
}

#include "Libraries/TinyXML/tinyxml.h"

Invoice DbInvoice::getInvoice(long long rowId)
{
    std::string query = "SELECT num, day, month, year, month_notif, data, "
        "recipient_id, recipient_name, recipient_phone, recipient_address "
        "FROM financial "
        "WHERE id = " + std::to_string(rowId);

    Db db(query);

    while (db.hasRows()) {
        
        int invNumber = db.asInt(0);

        Date invDate{
            db.asInt(1),
            db.asInt(2),
            db.asInt(3)
        };

        int monthNotif = db.asInt(4);


        //if it's from monthly notification, parse the xml data and return the result:
                    if (monthNotif) {

                        TiXmlDocument doc;
                        doc.Parse(db.asString(5).c_str(), 0, TIXML_ENCODING_UTF8);

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

        inv.recipient.bulstat = db.asString(6);
        inv.recipient.name = db.asString(7);
        inv.recipient.phone = db.asString(8);
        inv.recipient.address = db.asString(9);

        Parser::parse(db.asString(5), inv);

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
