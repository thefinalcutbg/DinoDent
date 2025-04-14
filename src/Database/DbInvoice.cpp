#include "DbInvoice.h"
#include "Database/Database.h"
#include "Model/User.h"
#include "Model/Parser.h"

#include <TinyXML/tinyxml.h>

long long DbInvoice::insertInvoice(const Invoice& invoice)
{ 
    Db db;

    if(invoice.nhifData.has_value()){

        db.newStatement(
            "INSERT INTO financial (practice_rzi, num, type, date, claimed_hash, recipient_id, data) "
            "VALUES (?,?,?,?,?,?,?)"
        );

        db.bind(1, User::practice().rziCode);
        db.bind(2, invoice.number);
        db.bind(3, static_cast<int>(invoice.type));
        db.bind(4, invoice.date.to8601());
        db.bind(5, invoice.nhifData->claimedHash);
        db.bind(6, invoice.recipient.identifier);
        db.bind(7, invoice.nhifData->monthNotifData);
    }
    else
    {
        db.newStatement(
            "INSERT INTO financial (practice_rzi, num, type, date, claimed_hash, data,"
            " recipient_id, recipient_name, recipient_address, recipient_phone, recipient_vat) "
            "VALUES (?,?,?,?,?,?,?,?,?,?, ?)"
        );

        db.bind(1, User::practice().rziCode);
        db.bind(2, invoice.number);
        db.bind(3, static_cast<int>(invoice.type));
        db.bind(4, invoice.date.to8601());
        db.bindNull(5);
        db.bind(6, Parser::write(invoice));
        db.bind(7, invoice.recipient.identifier);
        db.bind(8, invoice.recipient.name);
        db.bind(9, invoice.recipient.address);
        db.bind(10, invoice.recipient.phone);
        db.bind(11, invoice.recipient.hasVat);
    }

    db.execute();

    return db.lastInsertedRowID();


}

void DbInvoice::updateInvoice(const Invoice& invoice)
{
    Db db(
            "UPDATE financial SET "
            "num=?,"
            "type=?,"
            "date=?,"
            "data=?,"
            "recipient_id=?,"
            "recipient_name=?,"
            "recipient_phone=?,"
            "recipient_address=?, "
            "recipient_vat=? "
            "WHERE rowid=?"
    );

    db.bind(1, invoice.number);
    db.bind(2, static_cast<int>(invoice.type));
    db.bind(3, invoice.date.to8601());
    db.bind(4, Parser::write(invoice));
    db.bind(5, invoice.recipient.identifier);
        
    if (!invoice.nhifData.has_value()) {

        db.bind(6, invoice.recipient.name);
        db.bind(7, invoice.recipient.phone);
        db.bind(8, invoice.recipient.address);
        db.bind(9, invoice.recipient.hasVat);
    }

    db.bind(10, invoice.rowId);

    db.execute();
}

long long DbInvoice::invoiceAlreadyExists(const std::string& claimedHash)
{

     std::string query =
         "SELECT rowid FROM financial "
         "WHERE claimed_hash = ? " 
         "AND practice_rzi = ? "
         "ORDER BY num DESC LIMIT 1";

     Db db(query);

     db.bind(1, claimedHash);
     db.bind(2, User::practice().rziCode);

     while (db.hasRows()) {
         return db.asRowId(0);
     }

     return 0;
}

bool DbInvoice::invoiceAlreadyExists(long long number, long long rowid)
{
    std::string query{
        "SELECT num FROM financial WHERE "
        "num =" + std::to_string(number) + " "
        "AND practice_rzi = '" + User::practice().rziCode + "' "
        "AND rowid !=" + std::to_string(rowid)
    };

    for (Db db(query); db.hasRows();)
    {
        return true;
    }

    return false;

}

std::set<std::string> DbInvoice::getClaimedHashes()
{
    std::string query{
    "SELECT claimed_hash FROM financial WHERE "
    "practice_rzi = '" + User::practice().rziCode + "' " };

    std::set<std::string> result;

    for (Db db(query); db.hasRows();)
    {
        result.insert(db.asString(0));
    }

    return result;;
}

std::optional<Date> DbInvoice::getMainDocDate(long long invoiceNumber, const std::string& recipient_id)
{
    std::string query{
        "SELECT date FROM financial WHERE "
        "num = " + std::to_string(invoiceNumber) + " "
        "AND type = 0 "
        "AND recipient_id = '" + recipient_id + "' "
        "AND practice_rzi = '" + User::practice().rziCode + "'"
    };

    for (Db db(query); db.hasRows();)
    {
        return Date(db.asString(0));
    }

    return {};
}

std::optional<MainDocument> DbInvoice::getMainDocument(const std::string& recipient_id, long long currentRowid)
{
    std::string query{
    "SELECT num, date FROM financial WHERE "
    "type = 0 "
    "AND recipient_id = '" + recipient_id + "' "
    "AND practice_rzi = '" + User::practice().rziCode + "' "
    "AND rowid !="+std::to_string(currentRowid) + " "
    "ORDER BY num DESC LIMIT 1"
    };

    for (Db db(query); db.hasRows();)
    {
        return MainDocument{
            db.asLongLong(0),
            db.asString(1)
        };
    }

    return {};
}

std::optional<Recipient> DbInvoice::getRecipient(const std::string& bulstat)
{
    Db db(
        "SELECT recipient_id, recipient_name, recipient_address, recipient_phone, recipient_vat "
        "FROM financial WHERE recipient_id = ? "
        "ORDER BY num DESC LIMIT 1"
    );

    db.bind(1, bulstat);

    std::optional<Recipient> result;

    while (db.hasRows())
    {
        result.emplace();
        result->identifier = db.asString(0);
        result->name = db.asString(1);
        result->address = db.asString(2);
        result->phone = db.asString(3);
        result->hasVat = db.asBool(4);
    }

    return result;
}

Invoice DbInvoice::getInvoice(long long rowId)
{
    std::string query = "SELECT num, type, date, claimed_hash, data, "
        "recipient_id, recipient_name, recipient_phone, recipient_address, recipient_vat, claimed_hash IS NOT NULL AS isNhif "
        "FROM financial "
        "WHERE rowid = " + std::to_string(rowId);

    Db db(query);

    while (db.hasRows()) {

        long long invNumber = db.asLongLong(0);
        FinancialDocType type = static_cast<FinancialDocType>(db.asInt(1));
        Date invDate = db.asString(2);

        //if NHIF parse the xml data and return the result:
        if (db.asBool(10)) {

            TiXmlDocument doc;
            doc.Parse(db.asString(4).c_str(), 0, TIXML_ENCODING_UTF8);

            std::string claimedHash = db.asString(3);

            Invoice result(doc, claimedHash, User::practice(), User::doctor());

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

        Parser::parse(db.asString(4), inv);
        inv.recipient.identifier = db.asString(5);
        inv.recipient.name = db.asString(6);
        inv.recipient.phone = db.asString(7);
        inv.recipient.address = db.asString(8);
        inv.recipient.hasVat = db.asBool(9);

  

        return inv;
    }

    return Invoice();
}

long long DbInvoice::getNewInvoiceNumber()
{
    long long number = 0;

    Db db(
        "SELECT num FROM financial WHERE "
        "practice_rzi = '" + User::practice().rziCode + "' "
        "ORDER BY rowid DESC, num DESC LIMIT 1"
    );

    while (db.hasRows()) number = db.asLongLong(0);

    return number + 1;
}

std::unordered_set<int> DbInvoice::getExistingNumbers()
{
    std::unordered_set<int> result;

    Db db(
        "SELECT num FROM financial WHERE "
        "practice_rzi = '" + User::practice().rziCode + "' "
    );

    while (db.hasRows()) {
        result.insert(db.asInt(0));
    }

    return result;
}
