#include "DbPerio.h"
#include "Model/Tooth/ToothParser.h"

ToothContainer DbPerio::getStatus(const std::string& patientID, const Date& perioDate)
{
	openConnection();



    std::string jsonStatus;

    std::string query = "SELECT status_json FROM amblist WHERE "
        "patient_id = '" + patientID + "' "
        "AND month <= " + std::to_string(perioDate.month) + " "
        "AND year <= " + std::to_string(perioDate.year) + " "
        "ORDER BY id DESC LIMIT 1";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) != SQLITE_DONE)
    {
        jsonStatus = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    }


    sqlite3_finalize(stmt);

    closeConnection();

    if (jsonStatus.empty()) return ToothContainer();

    ToothContainer toothStatus;
    ToothParser parser;
    parser.parse(jsonStatus, toothStatus);

    return toothStatus;
   
}
