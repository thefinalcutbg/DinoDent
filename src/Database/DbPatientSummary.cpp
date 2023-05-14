#include "DbPatientSummary.h"
#include "Model/Dental/AmbList.h"
#include "Model/Parser.h"
#include "Database/DbProcedure.h"
#include "Database/Database.h"
#include <qdebug.h>
std::vector<TimeFrame> DbPatientSummary::getFrames(long long patientRowId)
{
    std::vector<TimeFrame> initialFrames;

    Db db(
        "SELECT rowid, num, nrn, lpk, date, status FROM amblist WHERE patient_rowid=? ORDER BY date ASC"
    );

    db.bind(1, patientRowId);

    while (db.hasRows())
    {
        initialFrames.emplace_back();

        auto& frame = initialFrames.back();

        frame.type = TimeFrameType::InitialAmb;
        frame.rowid = db.asRowId(0);
        frame.number = db.asString(1);

        auto nrn = db.asString(2);
        if (nrn.size()) frame.number = db.asString(2);

        frame.LPK = db.asString(3);
        frame.date = db.asString(4);
        Parser::parse(db.asString(5), frame.teeth);

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
                result.back().procedures.clear();
                result.back().type = TimeFrameType::Procedures;
            }

            procedures[i].applyProcedure(result.back().teeth);
            result.back().procedures.push_back(procedures[i]);
        }
    }

    return result;
}
