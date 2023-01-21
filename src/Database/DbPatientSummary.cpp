#include "DbPatientSummary.h"
#include "Model/Dental/AmbList.h"
#include "Model/Parser.h"
#include "Database/DbProcedure.h"
#include "Database/Database.h"

std::vector<TimeFrame> DbPatientSummary::getFrames(long long patientRowId)
{
    std::vector<TimeFrame> result;

    std::string query =
        "SELECT "
        "amblist.rowid,"
        "amblist.num,"
        "amblist.lpk,"
        "amblist.date,"
        "amblist.status,"
        "procedure.date,"
        "procedure.nzok,"  	
        "procedure.type,"  	
        "procedure.code,"  	
        "procedure.tooth," 	
     //   "procedure.price,"	    
        "procedure.data,"	    
        "procedure.deciduous, "
        "procedure.name, "
        "procedure.diagnosis, "
        "procedure.ksmp, "
        "procedure.notes, "
        "FROM amblist LEFT JOIN procedure ON "
        "amblist.rowid = procedure.amblist_rowid "
        "WHERE amblist.patient_rowid = " + std::to_string(patientRowId) + " "
        "ORDER BY procedure.date ASC, procedure.rowid ASC";

    Db db(query);

    long long lastRowid{ 0 };

    while (db.hasRows())
    {
        auto currentRowid = db.asRowId(0);

        if (currentRowid != lastRowid)
        {
            result.emplace_back();

            auto& frame = result.back();

            frame.type = TimeFrameType::InitialAmb;
            frame.rowid = currentRowid;
            frame.number = db.asString(1);
            frame.LPK = db.asString(2);
            frame.date = db.asString(3);
            Parser::parse(db.asString(4), frame.teeth);

            lastRowid = currentRowid;
        }
        
        auto procedureDate = db.asString(5);

        if (result.back().type == TimeFrameType::InitialAmb ||
            result.back().procedures[0].date != procedureDate
            )
        {
            result.emplace_back(result.back());
            result.back().type = TimeFrameType::Procedures;
            result.back().procedures.clear();
        }

        Procedure p;
        p.date = procedureDate;
        p.financingSource = static_cast<Procedure::FinancingSource>(db.asInt(6));
        p.type = static_cast<ProcedureType>(db.asInt(7));
        p.code = db.asInt(8);
        p.tooth = db.asInt(9);
   //     p.price = db.asDouble(10);
        Parser::parse(db.asString(10), p);
        p.temp = db.asInt(11);
        p.LPK = db.asString(2);
        p.name = db.asString(12);
        p.diagnosis = db.asString(13);
        p.ksmp = db.asString(14);
        p.notes = db.asString(15);
        result.back().procedures.push_back(p);
        p.applyProcedure(result.back().teeth);
        
    }

    return result;
}
