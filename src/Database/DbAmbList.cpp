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


long long DbAmbList::insert(const AmbList& sheet, long long patientRowId)
{

    Db db("INSERT INTO amblist "
        "(date, num, nhif, status, patient_rowid, lpk, rzi) "
        "VALUES (?,?,?,?,?,?,?)");

    db.bind(1, sheet.time.to8601(sheet.getDate()));
    db.bind(2, sheet.number);
    db.bind(3, Parser::write(sheet.nhifData, sheet.hasNZOKProcedure()));
    db.bind(4, Parser::write(sheet.teeth));
    db.bind(5, patientRowId);
    db.bind(6, sheet.LPK);
    db.bind(7, User::practice().rziCode);

    if (!db.execute()) {
        return 0;
    }

    auto rowID = db.lastInsertedRowID();

    db.execute("DELETE FROM procedure WHERE amblist_rowid = " + std::to_string(rowID));

    for (auto& p : sheet.procedures)
    {
        db.newStatement(
            "INSERT INTO procedure "
            "(date, nzok, type, code, tooth, deciduous, data, ksmp, amblist_rowid) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

        db.bind(1, p.date.to8601());
        db.bind(2, p.nhif);
        db.bind(3, static_cast<int>(p.type));
        db.bind(4, p.code);
        db.bind(5, p.tooth);
        db.bind(6, p.temp);
    //    db.bind(7, p.price);
        db.bind(7, Parser::write(p));
        db.bind(8, p.ksmp);
        db.bind(9, rowID);

        db.execute();
    }

    return rowID;

}

void DbAmbList::update(const AmbList& sheet)
{
    std::string query = "UPDATE amblist SET "
        "num=?,"
        "date=?,"
        "nhif=?,"
        "status=? "
        "WHERE rowid=?"
    ;
    
    Db db(query);


    db.bind(1, sheet.number);
    db.bind(2, sheet.time.to8601(sheet.getDate()));
    db.bind(3, Parser::write(sheet.nhifData, sheet.hasNZOKProcedure()));
    db.bind(4, Parser::write(sheet.teeth));
    db.bind(5, sheet.rowid);

    db.execute();

    db.execute("DELETE FROM procedure WHERE amblist_rowid = " + std::to_string(sheet.rowid));

    for (auto& p : sheet.procedures)
    {
        db.newStatement(
            "INSERT INTO procedure "
            "(nzok, type, code, date, tooth, deciduous, data, ksmp, amblist_rowid) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");

        db.bind(1, p.nhif);
        db.bind(2, static_cast<int>(p.type));
        db.bind(3, p.code);
        db.bind(4, p.date.to8601());
        db.bind(5, p.tooth);
        db.bind(6, p.temp);
      //  db.bind(7, p.price);
        db.bind(7, Parser::write(p));
        db.bind(8, p.ksmp);
        db.bind(9, sheet.rowid);

        db.execute();
    }
}

AmbList DbAmbList::getNewAmbSheet(long long patientRowId)
{

    AmbList ambList;
    ambList.LPK = User::doctor().LPK;
    std::string status;

    Db db;
    std::string query(
    
        "SELECT rowid, num, nhif, status FROM amblist WHERE "
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
        ambList.nhifData = Parser::parseNhifData(db.asString(2));
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

        auto procedures = DbProcedure::getProcedures(oldId, &db);

        for (auto& p : procedures)
        {
            p.applyProcedure(ambList.teeth);
        }
    }
    else
    {
        Parser::parse(status, ambList.teeth);
        ambList.procedures.addProcedures(DbProcedure::getProcedures(ambList.rowid, &db));
    }


    return ambList;
}

AmbList DbAmbList::getListData(long long rowId)
{

    std::string status;
    AmbList ambList;

    Db db(
        "SELECT rowid, num, nhif, status, patient_rowid, date FROM amblist WHERE "
        "rowid = " + std::to_string(rowId)
    );

    while (db.hasRows())
    {
        ambList.rowid = db.asRowId(0);
        ambList.number = db.asInt(1);
        ambList.nhifData = Parser::parseNhifData(db.asString(2));
        status = db.asString(3);
        ambList.LPK = User::doctor().LPK;
        ambList.patient_rowid = db.asRowId(4);
        ambList.time = db.asString(5);
    }

    Parser::parse(status, ambList.teeth);
    ambList.procedures.addProcedures(DbProcedure::getProcedures(ambList.rowid, &db));

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

    std::string condition = nhif ? "sum(procedure.nzok) > 0 " : "sum(procedure.nzok) = 0 ";

    query = 
        "SELECT amblist.num FROM amblist "
        "JOIN procedure ON amblist.rowid = procedure.amblist_rowid "
        "GROUP BY amblist.rowid "
        "HAVING "
        + condition +
        "AND strftime('%Y-%m-%d', amblist.date) BETWEEN '" + std::to_string(ambDate.year) + "-01-01'"
        "AND '" + ambDate.to8601() + "' "
        "AND amblist.lpk = '" + User::doctor().LPK + "' "
        "AND amblist.rzi = '" + User::practice().rziCode + "' "
        "ORDER BY amblist.num DESC LIMIT 1";

    int number = nhif ? 0 : 100000;

    for (Db db(query); db.hasRows();) {
        number = db.asInt(0);
    };

    return number + 1;

}

std::vector<AmbList> DbAmbList::getMonthlyNhifSheets(int month, int year)
{
    std::vector<AmbList> result;

     std::string query = 
        "SELECT "
        "amblist.rowid," 
        "amblist.patient_rowid,"
        "amblist.num,"
        "amblist.nhif,"
        "amblist.status,"
        "amblist.LPK,"
        "procedure.type,"	
        "procedure.code,"		
        "procedure.tooth,"		
        "procedure.date,"			
        "procedure.data,"		
        "procedure.deciduous,"
        "procedure.ksmp "		
        "FROM amblist JOIN procedure ON amblist.rowid = procedure.amblist_rowid "
        "WHERE procedure.nzok = 1 "
        "AND lpk = '" + User::doctor().LPK + "' "
        "AND rzi = '" + User::practice().rziCode + "' "
        "AND strftime('%m', amblist.date)='" + FreeFn::leadZeroes(month, 2) + "' "
        "AND strftime('%Y', amblist.date)='" + std::to_string(year) + "' "
        "ORDER BY amblist.num ASC";



     Db db(query);

     long long sheetRowid = 0;

     while (db.hasRows())
     {
         auto currentRowid = db.asRowId(0);

         if (currentRowid != sheetRowid) {

             result.emplace_back();

             auto& sheet = result.back();

             sheet.rowid = currentRowid;
             sheet.patient_rowid = db.asRowId(1);
             sheet.number = db.asInt(2);
             sheet.nhifData = Parser::parseNhifData(db.asString(3));
             Parser::parse(db.asString(4), sheet.teeth);

             sheetRowid = currentRowid;
             
         }

         Procedure p;

         p.nhif = true;
         p.LPK = db.asString(5);
         p.type = static_cast<ProcedureType>(db.asInt(6));
         p.code = db.asInt(7);
         p.tooth = db.asInt(8);
         p.date = db.asString(9);
         Parser::parse(db.asString(10), p);
         p.temp = db.asBool(11);
         p.ksmp = db.asString(12);
        

         result.back().procedures.addProcedure(p);

     }

     return result;

}
