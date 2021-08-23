#pragma once
#include "Model/Procedure/ProcedureParser.h"
#include "AbstractORM.h"
#include "sqLite/sqlite3.h"
#include <vector>
#include "Model/Procedure/MasterNZOK.h"
#include "Model/Procedure/TableStructs.h"


class DbProcedure : public AbstractORM
{
  
    ProcedureParser procedure_parser;
public:
    std::vector<Procedure> getProcedures(const std::string& amblist_id, const Date& amb_date);
    void saveProcedures(const std::string& amblist_id, const std::vector<Procedure>& mList);

    std::vector<ProcedureSummary> getSummary(const std::string& patientID, const std::string& excludeAmbIde);
    std::vector<DetailsSummary> getToothProcedures(const std::string& patientID, int tooth);
};

