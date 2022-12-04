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

    for (auto& r : sheet.referrals)
    {
        db.newStatement(
            "INSERT INTO referral "
            "(amblist_rowid, number, date, reason, main_diagnosis, additional_diagnosis, type, data) "
            "VALUES (?,?,?,?,?,?,?,?) "
        );

        db.bind(1, rowID);
        db.bind(2, r.number);
        db.bind(3, r.date.to8601());
        db.bind(4, r.reason.getIndex());
        db.bind(5, r.diagnosis.main.code());
        db.bind(6, r.diagnosis.additional.code());
        db.bind(7, static_cast<int>(r.type));
        
        auto data = r.type == ReferralType::MDD4 ? 
            std::to_string(std::get<MDD4Data>(r.data).tooth)
            : 
            std::string{};

        db.bind(8, data);

        db.execute();
    }

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

    db.execute("DELETE FROM procedure WHERE amblist_rowid = " + std::to_string(sheet.rowid));

    for (auto& p : sheet.procedures)
    {
        db.newStatement(
            "INSERT INTO procedure "
            "(nzok, type, code, date, tooth, deciduous, data, ksmp, amblist_rowid, name, diagnosis) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

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
        db.bind(10, p.name);
        db.bind(11, p.diagnosis);

        db.execute();
    }

    db.execute("DELETE FROM referral WHERE amblist_rowid = " + std::to_string(sheet.rowid));

    for (auto& r : sheet.referrals)
    {
        db.newStatement(
            "INSERT INTO referral "
            "(amblist_rowid, number, date, reason, main_diagnosis, additional_diagnosis, type, data) "
            "VALUES (?,?,?,?,?,?,?,?) "
        );

        db.bind(1, sheet.rowid);
        db.bind(2, r.number);
        db.bind(3, r.date.to8601());
        db.bind(4, r.reason.getIndex());
        db.bind(5, r.diagnosis.main.code());
        db.bind(6, r.diagnosis.additional.code());
        db.bind(7, static_cast<int>(r.type));

        auto data = r.type == ReferralType::MDD4 ?
            std::to_string(std::get<MDD4Data>(r.data).tooth)
            :
            std::string{};

        db.bind(8, data);

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
    ambList.procedures.addProcedures(DbProcedure::getProcedures(ambList.rowid, &db));


    db.newStatement(
        "SELECT referral.type, referral.number, referral.date, referral.reason,"
        "referral.main_diagnosis, referral.additional_diagnosis, referral.data "
        "FROM referral LEFT JOIN amblist ON referral.amblist_rowid=amblist.rowid "
        "WHERE amblist.rowid = ?"
    );

    db.bind(1, ambList.rowid);

    while (db.hasRows())
    {
        ambList.referrals.emplace_back(static_cast<ReferralType>(db.asInt(0)));
        
        auto& ref = ambList.referrals.back();

        ref.number = db.asInt(1);
        ref.date = db.asString(2);
        ref.reason = db.asInt(3);
        ref.diagnosis.main = db.asString(4);
        ref.diagnosis.additional = db.asString(5);
        
        if (ref.type == ReferralType::MDD4) {
           ref.data = MDD4Data(std::stoi(db.asString(6)));
        }

    }


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

     std::string query = 
        "SELECT "
        "amblist.rowid," 
        "amblist.patient_rowid,"
        "amblist.num,"
        "amblist.nhif_spec,"
        "amblist.status,"
        "amblist.LPK,"
        "procedure.type,"	
        "procedure.code,"		
        "procedure.tooth,"		
        "procedure.date,"			
        "procedure.name,"		
        "procedure.deciduous,"
        "procedure.ksmp, "
        "procedure.diagnosis "
        "FROM amblist JOIN procedure ON amblist.rowid = procedure.amblist_rowid "
        "WHERE amblist.nhif_spec IS NOT NULL "
        "AND lpk = '" + User::doctor().LPK + "' "
        "AND rzi = '" + User::practice().rziCode + "' "
        "AND strftime('%m', amblist.date)='" + FreeFn::leadZeroes(month, 2) + "' "
        "AND strftime('%Y', amblist.date)='" + std::to_string(year) + "' "
        "ORDER BY amblist.num ASC";


     long long sheetRowid = 0;

     for(Db db(query); db.hasRows();)
     {
         auto currentRowid = db.asRowId(0);

         if (currentRowid != sheetRowid) {

             result.emplace_back();

             auto& sheet = result.back();

             sheet.rowid = currentRowid;
             sheet.patient_rowid = db.asRowId(1);
             sheet.number = db.asInt(2);
             sheet.nhifData.specification = static_cast<NhifSpecification>(db.asInt(3));
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
         p.name = db.asString(10);
         p.temp = db.asBool(11);
         p.ksmp = db.asString(12);
         p.diagnosis = db.asString(13);
        
         result.back().procedures.addProcedure(p);

     }



     for (auto& sheet : result)
     {
         Db db;

         db.newStatement(
             "SELECT referral.type, referral.number, referral.date, referral.reason,"
             "referral.main_diagnosis, referral.additional_diagnosis, referral.data "
             "FROM referral LEFT JOIN amblist ON referral.amblist_rowid=amblist.rowid "
             "WHERE amblist.rowid = ?"
         );

         db.bind(1, sheet.rowid);

         while (db.hasRows())
         {
             sheet.referrals.emplace_back(static_cast<ReferralType>(db.asInt(0)));

             auto& ref = sheet.referrals.back();

             ref.number = db.asInt(1);
             ref.date = db.asString(2);
             ref.reason = db.asInt(3);
             ref.diagnosis.main = db.asString(4);
             ref.diagnosis.additional = db.asString(5);

             if (ref.type == ReferralType::MDD4) {
                 ref.data = MDD4Data(std::stoi(db.asString(6)));
             }

         }
     }
    
     return result;

}
