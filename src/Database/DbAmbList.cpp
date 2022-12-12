#include "DbAmbList.h"
#include "Database/Database.h"
#include "Model/User.h"
#include "Model/Patient.h"
#include "Model/Dental/AmbList.h"
#include "Model/Date.h"
#include "Model/Parser.h"
#include "DbProcedure.h"
#include <qdebug.h>
#include "Model/FreeFunctions.h"
#include "Database//DbReferral.h"

long long DbAmbList::insert(const AmbList& sheet, long long patientRowId)
{

    Db db("INSERT INTO amblist "
        "(date, num, nhif_spec, status, patient_rowid, lpk, rzi) "
        "VALUES (?,?,?,?,?,?,?)");

    db.bind(1, sheet.time.to8601(sheet.getDate()));
    db.bind(2, sheet.number);
    sheet.isNhifSheet() ? db.bind(3, static_cast<int>(sheet.nhifData.specification)) : db.bindNull(3);
    db.bind(4, Parser::write(sheet.teeth));
    db.bind(5, patientRowId);
    db.bind(6, sheet.LPK);
    db.bind(7, User::practice().rziCode);

    if (!db.execute()) {
        return 0;
    }

    auto rowID = db.lastInsertedRowID();

    DbProcedure::saveProcedures(rowID, sheet.procedures.list(), db);

    DbReferral::saveReferrals(sheet.referrals, rowID, db);

    return rowID;

}

void DbAmbList::update(const AmbList& sheet)
{
    std::string query = "UPDATE amblist SET "
        "num=?,"
        "date=?,"
        "nhif_spec=?,"
        "status=? "
        "WHERE rowid=?"
    ;
    
    Db db(query);


    db.bind(1, sheet.number);
    db.bind(2, sheet.time.to8601(sheet.getDate()));
    sheet.isNhifSheet() ? db.bind(3, static_cast<int>(sheet.nhifData.specification)) : db.bindNull(3);
    db.bind(4, Parser::write(sheet.teeth));
    db.bind(5, sheet.rowid);

    db.execute();

    DbProcedure::saveProcedures(sheet.rowid, sheet.procedures.list(), db);

    DbReferral::saveReferrals(sheet.referrals, sheet.rowid, db);

}

AmbList DbAmbList::getNewAmbSheet(long long patientRowId)
{

    AmbList ambList;
    ambList.LPK = User::doctor().LPK;
    std::string status;

    Db db;
    std::string query(
    
        "SELECT rowid, num, nhif_spec, status FROM amblist WHERE "
        "patient_rowid = " + std::to_string(patientRowId) + " AND "
        "lpk = '" + User::doctor().LPK + "' AND "
        "rzi = '" + User::practice().rziCode + "' AND "
        "strftime('%Y',amblist.date) = strftime('%Y',date('now')) AND  strftime('%m',amblist.date) = strftime('%m',date('now'))"
    
    );

    db.newStatement(query);

    while(db.hasRows())
    {
        ambList.patient_rowid = patientRowId;
        ambList.rowid = db.asRowId(0);
        ambList.number = db.asInt(1);
        ambList.nhifData.specification = static_cast<NhifSpecification>(db.asInt(2));
        status = db.asString(3);
    }

    if (ambList.isNew())
    {
        db.newStatement(
            
            "SELECT rowid, status FROM amblist WHERE "
            "patient_rowid = " + std::to_string(patientRowId) + " "
            "ORDER BY date DESC LIMIT 1"

            );

        long long oldId = 0;

        while(db.hasRows()){
            
            oldId = db.asRowId(0);
            status = db.asString(1);
        }

        if (!oldId) return ambList; //it means no data is found for this patient

        Parser::parse(status, ambList.teeth);

        auto procedures = DbProcedure::getProcedures(oldId, db);

        for (auto& p : procedures)
        {
            p.applyProcedure(ambList.teeth);
        }
    }
    else
    {
        Parser::parse(status, ambList.teeth);
        ambList.procedures.addProcedures(DbProcedure::getProcedures(ambList.rowid, db));
        ambList.referrals = DbReferral::getReferrals(ambList.rowid, db);
    }

    return ambList;
}

AmbList DbAmbList::getListData(long long rowId)
{

    std::string status;
    AmbList ambList;

    Db db(
        "SELECT rowid, num, nhif_spec, status, patient_rowid, date FROM amblist WHERE "
        "rowid = " + std::to_string(rowId)
    );

    while (db.hasRows())
    {
        ambList.rowid = db.asRowId(0);
        ambList.number = db.asInt(1);
        ambList.nhifData.specification = static_cast<NhifSpecification>(db.asInt(2));
        status = db.asString(3);
        ambList.LPK = User::doctor().LPK;
        ambList.patient_rowid = db.asRowId(4);
        ambList.time = db.asString(5);
    }

    Parser::parse(status, ambList.teeth);
    ambList.procedures.addProcedures(DbProcedure::getProcedures(ambList.rowid, db));

    ambList.referrals = DbReferral::getReferrals(ambList.rowid, db);


    return ambList;

}


void DbAmbList::deleteCurrentSelection(const std::string& ambID)
{
    Db::crudQuery("DELETE FROM amblist WHERE rowid = " + ambID + ")");
}

std::unordered_set<int> DbAmbList::getExistingNumbers(int currentYear)
{

    std::unordered_set<int> existingNumbers;

    std::string query = "SELECT num FROM amblist WHERE "
        "lpk = '" + User::doctor().LPK + "' "
        "AND rzi = '" + User::practice().rziCode + "' "
        "AND strftime('%Y',date)=" + std::to_string(currentYear);

    for (Db db(query);db.hasRows();) existingNumbers.emplace(db.asInt(0));

    return existingNumbers;
}

bool DbAmbList::suchNumberExists(int year, int ambNum, long long ambRowid)
{
    auto query =

        "SELECT COUNT(num) FROM amblist WHERE "
        "lpk = '" + User::doctor().LPK + "' "
        "AND rzi ='" + User::practice().rziCode + "' "
        "AND strftime('%Y',date)='" + std::to_string(year) + "' "
        "AND num =" + std::to_string(ambNum) + " "
        "AND rowid !=" + std::to_string(ambRowid)
        ;
    
    qDebug() << query.c_str();

    Db db {query};

    for (;db.hasRows();) {
        return db.asBool(0);
    }
   
    return false;

}

int DbAmbList::getNewNumber(Date ambDate, bool nhif)
{

    std::string query;

//    std::string condition = nhif ? "sum(procedure.nzok) > 0 " : "sum(procedure.nzok) = 0 ";

    query = nhif ?
        "SELECT num FROM amblist WHERE nhif_spec IS NOT NULL "
        :
        "SELECT num FROM amblist WHERE nhif_spec IS NULL ";

    query +=
        "AND strftime('%Y',date) = '" + std::to_string(ambDate.year) + "' "
        "AND lpk = ? "
        "AND rzi = ? "
        "ORDER BY num DESC LIMIT 1";

    int number = nhif ? 0 : 100000;

    Db db(query);

  //  db.bind(1, ambDate.year);
    db.bind(1, User::doctor().LPK);
    db.bind(2, User::practice().rziCode);

    while (db.hasRows()) {
        number = db.asInt(0);
    };

    return number + 1;

}

std::vector<AmbList> DbAmbList::getMonthlyNhifSheets(int month, int year)
{
    std::vector<AmbList> result;

    std::map<long long, int> sheetRowIdMap;

    //getting amb sheets

     std::string query = 
        "SELECT "
        "rowid," 
        "patient_rowid,"
        "num,"
        "nhif_spec,"
        "status,"
        "LPK "
        "FROM amblist "
        "WHERE amblist.nhif_spec IS NOT NULL "
        "AND lpk = '" + User::doctor().LPK + "' "
        "AND rzi = '" + User::practice().rziCode + "' "
        "AND strftime('%m', amblist.date)='" + FreeFn::leadZeroes(month, 2) + "' "
        "AND strftime('%Y', amblist.date)='" + std::to_string(year) + "' "
        "ORDER BY num ASC";

     Db db(query);



     while(db.hasRows())
     {
        result.emplace_back();
        auto& sheet = result.back();
        sheet.rowid = db.asRowId(0);
        sheet.patient_rowid = db.asRowId(1);
        sheet.number = db.asInt(2);
        sheet.nhifData.specification = static_cast<NhifSpecification>(db.asInt(3));
        Parser::parse(db.asString(4), sheet.teeth);
        sheet.LPK = db.asString(5);

        sheetRowIdMap[sheet.rowid] = result.size() - 1;
     }

 
        //getting procedures

        db.newStatement(
            "SELECT "
            "procedure.type,"
            "procedure.code,"
            "procedure.tooth,"
            "procedure.date,"
            "procedure.name,"
            "procedure.deciduous,"
            "procedure.ksmp, "
            "procedure.diagnosis,"
            "amblist.rowid "
            "FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
            "WHERE procedure.nzok = 1 "
            "AND amblist.nhif_spec IS NOT NULL "
            "AND amblist.lpk = '" + User::doctor().LPK + "' "
            "AND amblist.rzi = '" + User::practice().rziCode + "' "
            "AND strftime('%m', amblist.date)='" + FreeFn::leadZeroes(month, 2) + "' "
            "AND strftime('%Y', amblist.date)='" + std::to_string(year) + "' "
            "ORDER BY amblist.num ASC"
        );


        while (db.hasRows())
        {
            if (!sheetRowIdMap.count(db.asRowId(8))) continue;

            auto& sheet = result[sheetRowIdMap[db.asRowId(8)]];

            Procedure p;

            p.nhif = true;
            p.LPK = sheet.LPK;
            p.type = static_cast<ProcedureType>(db.asInt(0));
            p.code = db.asInt(1);
            p.tooth = db.asInt(2);
            p.date = db.asString(3);
            p.name = db.asString(4);
            p.temp = db.asBool(5);
            p.ksmp = db.asString(6);
            p.diagnosis = db.asString(7);

            sheet.procedures.addProcedure(p);
        }

        //getting referrals (inefficient :( );
        
        for (auto& sheet : result)
        {
           sheet.referrals = DbReferral::getReferrals(sheet.rowid, db);
        }


     return result;

}
