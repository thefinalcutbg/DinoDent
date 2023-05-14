#include "DbPatientSummary.h"
#include "Model/Dental/AmbList.h"
#include "Model/Parser.h"
#include "Model/Dental/PerioStatus.h"
#include "Database/DbProcedure.h"
#include "Database/Database.h"

std::vector<TimeFrame> DbPatientSummary::getFrames(long long patientRowId)
{
    std::vector<TimeFrame> initialFrames;

    //getting amb sheets

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

    //getting procedures

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


    //getting perio statuses

   db.newStatement("SELECT rowid, lpk, date, data FROM periostatus WHERE"
        " patient_rowid = " + std::to_string(patientRowId) +
        " ORDER BY date ASC"
       );

    while (db.hasRows())
    {
        TimeFrame perioFrame;
        perioFrame.perioData.rowid = db.asRowId(0);
        perioFrame.rowid = db.asRowId(0);
        perioFrame.type = TimeFrameType::Perio;
        perioFrame.date = db.asString(2);
        perioFrame.LPK = db.asString(1);
        Parser::parse(db.asString(3), perioFrame.perioData);
        

        for (int i = 0; i < result.size(); i++)
        {

            if (perioFrame.date < result[i].date || i == result.size() - 1)
            {
                if (i)  perioFrame.teeth = result[i - 1].teeth; //getting previous status

                if (i == result.size() - 1) result.push_back(perioFrame);
                else result.insert(result.begin() + i, perioFrame);

                break;
            }

        }
    }

    //copying perio data to all other time frames
    TimeFrame* lastPerioFrame{ nullptr };

    for (int i = 0; i < result.size(); i++)
    {
        if (result[i].type == TimeFrameType::Perio) {
            lastPerioFrame = &result[i];
            continue;
        }

        if (lastPerioFrame) {
            result[i].perioData = lastPerioFrame->perioData;
        }
    }

    return result;
}
