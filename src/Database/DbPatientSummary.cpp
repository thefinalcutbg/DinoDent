#include "DbPatientSummary.h"
#include "Model/Dental/AmbList.h"
#include "Model/Parser.h"
#include "Database/DbProcedure.h"
#include "Database/Database.h"

std::vector<TimeFrame> DbPatientSummary::getFrames(long long patientRowId)
{
    std::vector<TimeFrame> initialFrames;

    std::string query =
        "SELECT "
        "amblist.rowid,"
        "amblist.num,"
        "amblist.lpk,"
        "amblist.date,"
        "amblist.status,"
        "WHERE amblist.patient_rowid = " + std::to_string(patientRowId) + " "
        "ORDER BY ASC";

    Db db(query);

    long long lastRowid{ 0 };

    while (db.hasRows())
    {
        initialFrames.emplace_back();

        auto& frame = initialFrames.back();

        frame.type = TimeFrameType::InitialAmb;
        frame.rowid = db.asRowId(0);
        frame.number = db.asString(1);
        frame.LPK = db.asString(2);
        frame.date = db.asString(3);
        Parser::parse(db.asString(4), frame.teeth);

    }


    std::vector<TimeFrame> result;

    for (auto& initFrame : initialFrames)
    {
        result.push_back(initFrame);

        auto procedures = DbProcedure::getProcedures(initFrame.rowid, db);

        for (int i = 0; i < procedures.size(); i++)
        {
            if (
                i == 0 ||
                procedures[i].date != procedures[i-1].date    
            ) {
                result.push_back(result.back());
                result.back().type = TimeFrameType::Procedures;
            }

            procedures[i].applyProcedure(result.back().teeth);
            result.back().procedures.push_back(procedures[i]);
        }
    }

    return result;
}
