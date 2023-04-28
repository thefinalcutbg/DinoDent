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
        "procedure.financing_source,"  	
        "procedure.code,"  	
        "procedure.tooth," 		    
        "procedure.data,"	    
        "procedure.deciduous, "
        "procedure.diagnosis, "
        "procedure.notes, "
        "procedure.hyperdontic, "
        "procedure.additional_description "
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
        p.financingSource = static_cast<FinancingSource>(db.asInt(6));
        p.code = db.asString(7);
        p.tooth = db.asInt(8);
        Parser::parse(db.asString(9), p);
        p.temp = db.asInt(10);
        p.LPK = db.asString(2);
        p.diagnosis = db.asString(11);
        p.notes = db.asString(12);
        p.hyperdontic = db.asBool(13);
        p.diagnosis.additionalDescription = db.asString(14);
        result.back().procedures.push_back(p);
        p.applyProcedure(result.back().teeth);
        
    }

    return result;
}
