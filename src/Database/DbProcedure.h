#pragma once

#include "Model/Procedure/TableStructs.h"
#include "Model/Procedure/Procedure.h"
#include <vector>

class Db;

namespace DbProcedure
{

    std::vector<Procedure> getProcedures(const std::string& amblist_id, Db* existingConnection = nullptr);
    void saveProcedures(const std::string& amblist_id, const std::vector<Procedure>& mList, Db* existingConnection = nullptr);

    std::vector<ProcedureSummary> getSummary(long long patientRowId, const std::string& excludeAmbIde);
    std::vector<Procedure> getToothProcedures(long long patientRowId, int tooth);
};

