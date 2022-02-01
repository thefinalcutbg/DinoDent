#include "DbPatientSummary.h"
#include "Model/AmbList.h"
#include "Model/Parser/Parser.h"
#include "Database/DbProcedure.h"
#include "Database/DbProcedure.h"

std::vector<TimeFrame> DbPatientSummary::getFrames(const std::string& patientID)
{
    openConnection();

    std::vector<TimeFrame> timeFrames{ TimeFrame{} }; //allocating the first element as default status

    std::string query =
        "SELECT "
        "amblist.id, "
        "amblist.lpk, "
        "procedure.day, "
        "amblist.month, "
        "amblist.year, "
        "amblist.status_json "

        "FROM amblist LEFT JOIN procedure ON "
        "amblist.id = procedure.amblist_id "
        "WHERE amblist.patient_id = '" + patientID + "' "
        "GROUP BY procedure.day, amblist.month, amblist.year "
        "ORDER BY amblist.year ASC, amblist.month ASC, amblist.day ASC";


    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {


        timeFrames.emplace_back(
            TimeFrame
            {
               std::string{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))},
               reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),

               Date{
                    sqlite3_column_int(stmt, 2),
                    sqlite3_column_int(stmt, 3),
                    sqlite3_column_int(stmt, 4)
               }, 
               
               {} //allocating ToothContainer with default constructor
                
            }

             );
      
            Parser::parse(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5))), timeFrames.back().teeth);
    }

    sqlite3_finalize(stmt);

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
        "procedure.temp, "	//9
        "amblist.LPK, "		//10
        "amblist.id "       //11
        "FROM procedure LEFT JOIN amblist ON procedure.amblist_id = amblist.id "
        "WHERE amblist.patient_id = '" + patientID + "' " 
        "ORDER BY amblist.year ASC, amblist.month ASC, amblist.day ASC, seq ASC";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    int tfIdx = 1; //time frame index - the 0 position is reserved for the initial status;

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        
        Procedure p;

        p.nzok = sqlite3_column_int(stmt, 0);
        p.type = static_cast<ProcedureType>(sqlite3_column_int(stmt, 1));
        p.code = sqlite3_column_int(stmt, 2);
        p.tooth = sqlite3_column_int(stmt, 3);
        p.date.day = sqlite3_column_int(stmt, 4);
        p.date.month = sqlite3_column_int(stmt, 5);
        p.date.year = sqlite3_column_int(stmt, 6);
        p.price = sqlite3_column_double(stmt, 7);
        Parser::parse(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8))), p);
        p.temp = sqlite3_column_int(stmt, 9);
        p.LPK = UserManager::getDoctorName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 10)));
        
        while (p.date != timeFrames[tfIdx].date)
           tfIdx++;

        timeFrames[tfIdx].procedures.push_back(p);

    }

    sqlite3_finalize(stmt);

    closeConnection();

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
