#include "DbPatientSummary.h"
#include "Model/AmbList.h"
#include "Model/Parser/Parser.h"
#include "Database/DbProcedure.h"
#include "Database/Database.h"

std::vector<TimeFrame> DbPatientSummary::getFrames(long long patientRowId)
{

    std::vector<TimeFrame> timeFrames{ TimeFrame{} }; //allocating the first element as default status

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
               db.asString(0),
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
    }


    //the element 0 always shows initial status, so we copy status[1] onto status [0]
    if (timeFrames.size() > 1)
        timeFrames[0] = timeFrames[1];

    
    query = "SELECT	"
        "procedure.nzok, "	//0
        "procedure.type, "	//1
        "procedure.code, "	//2
        "procedure.tooth, "	//3
        "procedure.day, "	//4
        "amblist.month, "	//5
        "amblist.year,	"	//6
        "procedure.price, "	//7
        "procedure.data, "	//8
        "procedure.deciduous, "	//9
        "amblist.LPK, "		//10
        "amblist.rowid "       //11
        "FROM procedure LEFT JOIN amblist ON procedure.amblist_rowid = amblist.rowid "
        "WHERE amblist.patient_rowid = " + std::to_string(patientRowId) + " "
        "ORDER BY amblist.year ASC, amblist.month ASC, amblist.day ASC, procedure.rowid ASC";

    db.newStatement(query);

    int tfIdx = 1; //time frame index - the 0 position is reserved for the initial status;

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
        
        while (p.date != timeFrames[tfIdx].date)
           tfIdx++;

        timeFrames[tfIdx].procedures.push_back(p);

    }


    //the tooth statuses and procedures in their respective dates are allocated and sorted, but now
    //we have to apply them:

    for (int i = 1; i < timeFrames.size(); i++)
    {
        if (timeFrames[i].ambId == timeFrames[i - 1].ambId)
                 timeFrames[i].teeth = timeFrames[i - 1].teeth;

        for (auto& procedure : timeFrames[i].procedures)
                 procedure.applyProcedure(timeFrames[i].teeth);

    }

    return timeFrames;
}
