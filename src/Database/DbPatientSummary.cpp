#include "DbPatientSummary.h"
#include "Model/AmbList.h"
#include "Model/Parser/Parser.h"
#include "Database/DbProcedure.h"
#include "Database/Database.h"

std::vector<TimeFrame> DbPatientSummary::getFrames(long long patientRowId)
{

    std::vector<TimeFrame> timeFrames;

    std::string query =
        "SELECT "
        "amblist.rowid, "
        "amblist.lpk, "
        "procedure.day, "
        "amblist.month, "
        "amblist.year, "
        "amblist.status_json "

        "FROM amblist LEFT JOIN procedure ON "
        "amblist.rowid = procedure.amblist_rowid "
        "WHERE amblist.patient_rowid = " + std::to_string(patientRowId) + " "
        "GROUP BY procedure.day, amblist.month, amblist.year "
        "ORDER BY amblist.year ASC, amblist.month ASC, amblist.day ASC";

    Db db(query);

    while (db.hasRows())
    {
        timeFrames.emplace_back(
            TimeFrame
            {
               TimeFrameType::Procedures,
               db.asRowId(0),
               db.asString(1),

               Date{
                    db.asInt(2),
                    db.asInt(3),
                    db.asInt(4)
               }, 
               
               {} //allocating ToothContainer with default constructor
                
            }

             );
      
            Parser::parse(db.asString(5), timeFrames.back().teeth);

            //inserting InitialAmb if necessary:
            int currentIdx = timeFrames.size() - 1;
            
            if (currentIdx == 0 
                ||
                timeFrames[currentIdx - 1].rowid != timeFrames[currentIdx].rowid)
            {
                timeFrames.back().type = TimeFrameType::InitialAmb;
                timeFrames.push_back(timeFrames.back());
                timeFrames.back().type = TimeFrameType::Procedures;

            }
    }

    
    query = "SELECT	"
        "procedure.nzok, "  	//0
        "procedure.type, "  	//1
        "procedure.code, "  	//2
        "procedure.tooth, " 	//3
        "procedure.day, "	    //4
        "amblist.month, "	    //5
        "amblist.year,	"	    //6
        "procedure.price, "	    //7
        "procedure.data, "	    //8
        "procedure.deciduous, "	//9
        "amblist.LPK, "		    //10
        "amblist.rowid "        //11
        "FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
        "WHERE amblist.patient_rowid = " + std::to_string(patientRowId) + " "
        "ORDER BY amblist.year ASC, amblist.month ASC, procedure.day ASC, procedure.rowid ASC";

    db.newStatement(query);



    while (db.hasRows())
    {
        
        Procedure p;

        p.nzok = db.asInt(0);
        p.type = static_cast<ProcedureType>(db.asInt(1));
        p.code = db.asInt(2);
        p.tooth = db.asInt(3);
        p.date.day = db.asInt(4);
        p.date.month = db.asInt(5);
        p.date.year = db.asInt(6);
        p.price = db.asDouble(7);
        Parser::parse(db.asString(8), p);
        p.temp = db.asInt(9);
        p.LPK = db.asString(10);
    
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
