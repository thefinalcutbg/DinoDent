#include "DbAmbList.h"
#include "Database/Database.h"
#include "Model/User.h"
#include "Model/Patient.h"
#include "Model/Dental/AmbList.h"
#include "Model/Date.h"
#include "Model/Parser.h"
#include "DbProcedure.h"
#include "Model/FreeFunctions.h"
#include "Database//DbReferral.h"

long long DbAmbList::insert(const AmbList& sheet, long long patientRowId)
{

    Db db("INSERT INTO amblist "
        "(date, nrn, lrn, his_updated, basedOnNrn, num, nhif_spec, nhif_unfav, status, patient_rowid, lpk, rzi) "
        "VALUES (?,?,?,?,?,?,?,?,?,?,?,?)");

    bool sheetIsNhif = sheet.isNhifSheet();

    db.bind(1, sheet.date);
    db.bind(2, sheet.nrn);
    db.bind(3, sheet.lrn);
    db.bind(4, sheet.his_updated);
    db.bind(5, sheet.basedOn);
    db.bind(6, sheet.number);
    sheetIsNhif ? db.bind(7, static_cast<int>(sheet.nhifData.specification)) : db.bindNull(7);
    sheetIsNhif ? db.bind(8, sheet.nhifData.isUnfavourable) : db.bindNull(8);
    db.bind(9, Parser::write(sheet.teeth));
    db.bind(10, patientRowId);
    db.bind(11, sheet.LPK);
    db.bind(12, User::practice().rziCode);

    if (!db.execute()) {
        return 0;
    }

    auto rowID = db.lastInsertedRowID();

    DbProcedure::saveProcedures(rowID, sheet.procedures.list(), sheet.procedures.removedProcedures(), db);

    DbReferral::saveReferrals(sheet.referrals, rowID, db);

    return rowID;

}

void DbAmbList::update(const AmbList& sheet)
{
    std::string query = "UPDATE amblist SET "
        "nrn=?,"
        "his_updated=?,"
        "num=?,"
        "date=?,"
        "nhif_spec=?,"
        "nhif_unfav=?,"
        "status=? "
        "WHERE rowid=?"
    ;
    
    Db db(query);

    bool sheetIsNhif = sheet.isNhifSheet();
    db.bind(1, sheet.nrn);
    db.bind(2, sheet.his_updated);
    db.bind(3, sheet.number);
    db.bind(4, sheet.date);
    sheetIsNhif ? db.bind(5, static_cast<int>(sheet.nhifData.specification)) : db.bindNull(5);
    sheetIsNhif ? db.bind(6, sheet.nhifData.isUnfavourable) : db.bindNull(6);
    db.bind(7, Parser::write(sheet.teeth));
    db.bind(8, sheet.rowid);

    db.execute();

    DbProcedure::saveProcedures(sheet.rowid, sheet.procedures.list(), sheet.procedures.removedProcedures(), db);
    DbReferral::saveReferrals(sheet.referrals, sheet.rowid, db);

}

AmbList DbAmbList::getNewAmbSheet(long long patientRowId)
{

    AmbList ambList;
    ambList.LPK = User::doctor().LPK;
    std::string status;

    Db db;
    std::string query(
    
        "SELECT rowid, nrn, lrn, his_updated, basedOnNrn, num, nhif_spec, status FROM amblist WHERE "
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
        ambList.nrn = db.asString(1);
        ambList.lrn = db.asString(2);
        ambList.his_updated = db.asInt(3);
        ambList.basedOn = db.asString(4);
        ambList.number = db.asInt(5);
        ambList.nhifData.specification = static_cast<NhifSpecification>(db.asInt(6));
        status = db.asString(7);
    }

    if (ambList.isNew())
    {
        db.newStatement(
            
            "SELECT rowid, nrn, status FROM amblist WHERE "
            "patient_rowid = " + std::to_string(patientRowId) + " "
            "ORDER BY date DESC LIMIT 1"

            );

        long long oldId = 0;
        std::string basedOnNrn;

        while(db.hasRows()){
            
            oldId = db.asRowId(0);
            basedOnNrn = db.asString(1);
            status = db.asString(2);
        }

        if (!oldId) return ambList; //no data is found for this patient

        Parser::parse(status, ambList.teeth);

        auto procedures = DbProcedure::getProcedures(oldId, db);

        for (auto& p : procedures)
        {
            p.applyProcedure(ambList.teeth);
        }

        ambList.basedOn = basedOnNrn;

        return ambList;
    }

    Parser::parse(status, ambList.teeth);
    ambList.procedures.addProcedures(DbProcedure::getProcedures(ambList.rowid, db));
    ambList.procedures.setRemovedProcedures(DbProcedure::getProcedures(ambList.rowid, db, false, true));
    ambList.referrals = DbReferral::getReferrals(ambList.rowid, db);


    return ambList;
}

AmbList DbAmbList::getListData(long long rowId)
{

    std::string status;
    AmbList ambList;

    Db db(
        "SELECT rowid, nrn, lrn, his_updated, basedOnNrn, num, nhif_spec, nhif_unfav, status, patient_rowid, date FROM amblist WHERE "
        "rowid = " + std::to_string(rowId)
    );

    while (db.hasRows())
    {
        ambList.rowid = db.asRowId(0);
        ambList.nrn = db.asString(1);
        ambList.lrn = db.asString(2);
        ambList.his_updated = db.asBool(3);
        ambList.basedOn = db.asString(4);
        ambList.number = db.asInt(5);
        ambList.nhifData.specification = static_cast<NhifSpecification>(db.asInt(6));
        ambList.nhifData.isUnfavourable = db.asBool(7);
        status = db.asString(8);
        ambList.LPK = User::doctor().LPK;
        ambList.patient_rowid = db.asRowId(9);
        ambList.date = db.asString(10);
    }

    Parser::parse(status, ambList.teeth);
    ambList.procedures.addProcedures(DbProcedure::getProcedures(ambList.rowid, db));
    ambList.procedures.setRemovedProcedures(DbProcedure::getProcedures(ambList.rowid, db, false, true));
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
        "AND nrn IS NULL OR nrn = ''"
        "AND num =" + std::to_string(ambNum) + " "
        "AND rowid !=" + std::to_string(ambRowid)
        ;
    
    Db db {query};

    for (;db.hasRows();) {
        return db.asBool(0);
    }
   
    return false;

}

int DbAmbList::getNewNumber(Date ambDate, bool nhif)
{

    std::string query = 
        "SELECT num FROM amblist WHERE " 
        "strftime('%Y',date)=? "
        "AND lpk=? "
        "AND rzi=? "
        "AND nrn IS NULL OR nrn = '' "
        "ORDER BY num DESC LIMIT 1";

    Db db(query);

    db.bind(1, std::to_string(ambDate.year));
    db.bind(2, User::doctor().LPK);
    db.bind(3, User::practice().rziCode);

    while (db.hasRows()) {
        return db.asInt(0) + 1;
    };

    return 1;

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
        "nrn,"
        "nhif_spec,"
        "nhif_unfav,"
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
        sheet.nrn = db.asString(3);
        sheet.nhifData.specification = static_cast<NhifSpecification>(db.asInt(4));
        sheet.nhifData.isUnfavourable = db.asInt(5);
        Parser::parse(db.asString(6), sheet.teeth);
        sheet.LPK = db.asString(7);

        sheetRowIdMap[sheet.rowid] = result.size() - 1;
     }

 
    //getting procedures

    db.newStatement(
        "SELECT "
        "procedure.code,"
        "procedure.tooth,"
        "procedure.date,"
        "procedure.deciduous,"
        "procedure.diagnosis,"
        "procedure.hyperdontic,"
        "amblist.rowid "
        "FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
        "WHERE "
        "procedure.financing_source=" + std::to_string(static_cast<int>(FinancingSource::NHIF)) + " "
        "AND amblist.nhif_spec IS NOT NULL "
        "AND procedure.removed = 0 "
        "AND amblist.lpk = '" + User::doctor().LPK + "' "
        "AND amblist.rzi = '" + User::practice().rziCode + "' "
        "AND strftime('%m', amblist.date)='" + FreeFn::leadZeroes(month, 2) + "' "
        "AND strftime('%Y', amblist.date)='" + std::to_string(year) + "' "
        "ORDER BY amblist.num ASC"
    );


    while (db.hasRows())
    {
        if (!sheetRowIdMap.count(db.asRowId(6))) continue;

        auto& sheet = result[sheetRowIdMap[db.asRowId(6)]];

        Procedure p;

        p.financingSource = FinancingSource::NHIF;
        p.LPK = sheet.LPK;
        p.code = db.asString(0);
        p.tooth = db.asInt(1);
        p.date = db.asString(2);
        p.temp = db.asBool(3);
        p.diagnosis = db.asString(4);
        p.hyperdontic = db.asBool(5);

        sheet.procedures.addProcedure(p);
    }

    //getting referrals (inefficient :( );
        
    for (auto& sheet : result)
    {
        sheet.referrals = DbReferral::getReferrals(sheet.rowid, db);
    }


     return result;

}

void DbAmbList::updateNrn(const std::string& nrn, long long ambRowId)
{
    Db db("UPDATE amblist SET nrn=? WHERE rowid=?");

    nrn.empty() ? db.bindNull(1) : db.bind(1, nrn);

    db.bind(2, ambRowId);

    db.execute();
}
