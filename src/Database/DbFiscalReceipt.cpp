#include "DbFiscalReceipt.h"
#include "Database/Database.h"
#include "Model/FreeFunctions.h"
#include "Model/User.h"

std::string DbFiscalReceipt::getFiscalMemory()
{
    std::string query =
        "SELECT fiscal_memory "
        "FROM fiscal_receipt LEFT JOIN amblist "
        "ON fiscal_receipt.amblist_rowid = amblist.rowid "
        "WHERE amblist.rzi=? "
        "ORDER BY fiscal_receipt.timestamp DESC LIMIT 1";

    Db db(query);

    db.bind(1, User::practice().rziCode);

    while (db.hasRows()) return db.asString(0);

    return std::string();

}

std::vector<FiscalReceipt> DbFiscalReceipt::getReceipts(int month, int year)
{

    std::vector<FiscalReceipt> result;

    Db db(
        "SELECT "
        "fiscal_receipt.amblist_rowid, "
        "fiscal_receipt.timestamp, "
        "fiscal_receipt.fiscal_memory, "
        "fiscal_receipt.receipt_num, "
        "amblist.num, "
        "amblist.nrn, "
        "patient.id "
        "FROM fiscal_receipt LEFT JOIN amblist ON fiscal_receipt.amblist_rowid = amblist.rowid "
        "LEFT JOIN patient ON amblist.patient_rowid = patient.rowid "
        "WHERE "
        "strftime('%m', fiscal_receipt.timestamp)='" + FreeFn::leadZeroes(month, 2) + "' "
        "AND strftime('%Y', fiscal_receipt.timestamp)='" + std::to_string(year) + "' "
        "AND amblist.rzi=? "
        "ORDER BY fiscal_receipt.timestamp ASC"
    );

    db.bind(1, User::practice().rziCode);

    while (db.hasRows())
    {
        result.push_back(FiscalReceipt());
        auto& f = result.back();
        f.amblistRowid = db.asRowId(0);
        f.timestamp = db.asString(1);
        f.fiscal_memory = db.asString(2);
        f.receipt_num = db.asString(3);
        f.amblistNumber = db.asString(4);
        auto nrn = db.asString(5);

        if (nrn.size()) f.amblistNumber = nrn;

        f.patientId = db.asString(6);
    }

    return result;

}

bool DbFiscalReceipt::alreadyExists(long long ambRowid, const std::string& timestamp)
{
    Db db(
        "SELECT COUNT(*) FROM fiscal_receipt WHERE "
        "fiscal_receipt.amblist_rowid=? "
        "AND strftime('%Y-%m-%d',fiscal_receipt.timestamp)=strftime('%Y-%m-%d',?)"
    );

    db.bind(1, ambRowid);
    db.bind(2, timestamp);

    while (db.hasRows()) {
        return db.asInt(0);
    }

    return false;
}

void DbFiscalReceipt::saveReceipt(const FiscalReceipt& r)
{
    Db db(
        "INSERT INTO fiscal_receipt (timestamp, amblist_rowid, fiscal_memory, receipt_num) VALUES (?,?,?,?)"
    );

    db.bind(1, r.timestamp);
    db.bind(2, r.amblistRowid);
    db.bind(3, r.fiscal_memory);
    db.bind(4, r.receipt_num);

    db.execute();
}

void DbFiscalReceipt::deleteReceipt(long long ambRowid, const std::string& timestamp)
{
    Db db(
        "DELETE FROM fiscal_receipt WHERE amblist_rowid=? AND timestamp=?"
    );

    db.bind(1, ambRowid);
    db.bind(2, timestamp);
    db.execute();
}
