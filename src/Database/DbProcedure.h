#pragma once

#include "AbstractORM.h"

#include "Model/Procedure/MasterNZOK.h"
#include "Model/Procedure/TableStructs.h"
#include "Model/Procedure/Procedure.h"
#include <vector>

class DbProcedure : public AbstractORM
{

public:
    std::vector<Procedure> getProcedures(const std::string& amblist_id);
    void saveProcedures(const std::string& amblist_id, const std::vector<Procedure>& mList);

    std::vector<ProcedureSummary> getSummary(const std::string& patientID, const std::string& excludeAmbIde);
    std::vector<Procedure> getToothProcedures(const std::string& patientID, int tooth);
};

