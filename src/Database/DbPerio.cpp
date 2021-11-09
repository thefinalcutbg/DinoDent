#include "DbPerio.h"
#include "Model/Parser/Parser.h"


ToothContainer DbPerio::getStatus(const std::string& patientID, const Date& perioDate)
{
	openConnection();

    std::string jsonStatus;
    std::string amblistId;
    std::string LPK;

    std::string query = "SELECT status_json, id, LPK FROM amblist WHERE "
        "patient_id = '" + patientID + "' "
        "AND month <= " + std::to_string(perioDate.month) + " "
        "AND year <= " + std::to_string(perioDate.year) + " "
        "ORDER BY id DESC LIMIT 1";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        jsonStatus = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        amblistId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        LPK = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    }

    sqlite3_finalize(stmt);

    if (jsonStatus.empty())
    {
        closeConnection();
        return ToothContainer();
    }
    
    ToothContainer toothStatus;

    Parser::parse(jsonStatus, toothStatus);

    query = "SELECT type, tooth, temp, data FROM procedure WHERE "
            "amblist_id = '" + amblistId + "' "
            "AND day <= " + std::to_string(perioDate.day) + " "
            "ORDER BY seq";



    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);


    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        Procedure p;
        p.LPK = LPK;
        p.type = static_cast<ProcedureType>(sqlite3_column_int(stmt, 0));
        p.tooth = sqlite3_column_int(stmt, 1);
        p.temp = sqlite3_column_int(stmt, 2);
        Parser::parse(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))), p);
        p.applyProcedure(toothStatus);
    }

    closeConnection();

    return toothStatus;
   
}
