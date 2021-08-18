#pragma once
#include "Model/Procedure/ProcedureParser.h"
#include "AbstractORM.h"
#include "sqLite/sqlite3.h"
#include <vector>
#include "Model/Procedure/MasterNZOK.h"

#include <QDebug>

struct ProcedureSummary
{
    Date date;
    int code;
    int tooth;

};

class DbProcedure : public AbstractORM
{
  
    ProcedureParser tooth_pareser;
public:
    std::vector<Procedure> getManipulations(const std::string& amblist_id, const Date& amb_date);
    void saveManipulations(const std::string& amblist_id, const std::vector<Procedure>& mList);

    std::vector<ProcedureSummary> getSummary(const std::string& patientID, const std::string& excludeAmbIde);
};

