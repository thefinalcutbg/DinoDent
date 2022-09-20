#include "DbPatientSummary.h"
#include "Model/Dental/AmbList.h"
#include "Model/Parser.h"
#include "Database/DbProcedure.h"
#include "Database/Database.h"
#include "qdebug.h"
std::vector<TimeFrame> DbPatientSummary::getFrames(long long patientRowId)
{

    std::vector<TimeFrame> timeFrames;

    std::string query =
        "SELECT "
        "amblist.rowid,"
        "amblist.num,"
        "amblist.lpk,"
        "amblist.date,"
        "amblist.status, "
        "procedure.date "

        "FROM amblist LEFT JOIN procedure ON "
        "amblist.rowid = procedure.amblist_rowid "
        "WHERE amblist.patient_rowid = " + std::to_string(patientRowId) + " "
        "GROUP BY procedure.date "
        "ORDER BY procedure.date ASC";

    
    Db db(query);
    
    while (db.hasRows())
    {
        long long rowid = db.asRowId(0);

        if (timeFrames.empty() || timeFrames.back().rowid != rowid)
        {
            timeFrames.push_back(
                TimeFrame
                {
                   TimeFrameType::InitialAmb,
                   rowid,
                   db.asString(1),
                   db.asString(2),
                   Date{db.asString(3)}
                }
            );

            Parser::parse(db.asString(4), timeFrames.back().teeth);
        }

        timeFrames.push_back(timeFrames.back());
        timeFrames.back().type = TimeFrameType::Procedures;
        timeFrames.back().date = db.asString(5);
    }

    
    query = "SELECT	"
        "procedure.nzok, "  	//0
        "procedure.type, "  	//1
        "procedure.code, "  	//2
        "procedure.tooth, " 	//3
        "procedure.date, "	    //4
        "procedure.price, "	    //5
        "procedure.data, "	    //6
        "procedure.deciduous, "	//7
        "amblist.LPK "		    //8
        "FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
        "WHERE amblist.patient_rowid = " + std::to_string(patientRowId) + " "
        "ORDER BY procedure.date ASC, procedure.rowid ASC";

    db.newStatement(query);


    while (db.hasRows())
    {
        Procedure p;
        p.nzok = db.asInt(0);
        p.type = static_cast<ProcedureType>(db.asInt(1));
        p.code = db.asInt(2);
        p.tooth = db.asInt(3);
        p.date = db.asString(4);
        p.price = db.asDouble(5);
        Parser::parse(db.asString(6), p);
        p.temp = db.asInt(7);
        p.LPK = db.asString(8);
    
        for (int i = 0; i < timeFrames.size(); i++) { //optimize it!

            if (timeFrames[i].type == TimeFrameType::InitialAmb ||
                timeFrames[i].date != p.date
                ) {
                continue;
            }

            timeFrames[i].procedures.push_back(p);

        }    

    }


    //the tooth statuses and procedures in their respective dates are allocated and sorted, but now
    //we have to apply them:
    
    for (int i = 0; i < timeFrames.size(); i++)
    {
        if (timeFrames[i].type == TimeFrameType::InitialAmb) continue;

        if (timeFrames[i].rowid == timeFrames[i - 1].rowid)
                 timeFrames[i].teeth = timeFrames[i - 1].teeth;

        for (auto& procedure : timeFrames[i].procedures)
                 procedure.applyProcedure(timeFrames[i].teeth);

    }

    return timeFrames;
}
