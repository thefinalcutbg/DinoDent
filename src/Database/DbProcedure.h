#pragma once

#include "Model/Procedure/TableStructs.h"
#include "Model/Procedure/Procedure.h"
#include <vector>

class Date;

class Db;

namespace DbProcedure
{

    std::vector<Procedure> getProcedures(long long amblist_rowid, Db* existingConnection = nullptr);
    void saveProcedures(long long amblist_rowid, const std::vector<Procedure>& mList, Db* existingConnection = nullptr);

    std::vector<ProcedureSummary> getNhifSummary(
        long long patientRowId,
        long long excludeAmbRowId, 
        const Date& fromDate, 
        const Date& toDate
    );
    std::vector<Procedure> getToothProcedures(long long patientRowId, int tooth);
};

