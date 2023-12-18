#pragma once

#include "Model/TableStructs.h"
#include "Model/Dental/Procedure.h"
#include <vector>

class Date;

class Db;

namespace DbProcedure
{

    std::vector<Procedure> getProcedures(long long amblist_rowid, Db& db, bool nhifOnly = false, bool removed = false);
    std::vector<Procedure>  getProcedures(long long amblist_rowid, bool nhifOnly = false, bool removed = false);
    void saveProcedures(long long amblist_rowid, const std::vector<Procedure>& p, const std::vector<Procedure> deleted, Db& db);
    std::vector<ProcedureSummary> getNhifSummary(
        long long patientRowId,
        long long excludeAmbRowId, 
        const Date& fromDate, 
        const Date& toDate
    );
    std::vector<Procedure> getToothProcedures(long long patientRowId, int tooth);
    std::vector<Procedure> getPatientProcedures(long long patientRowid);
    std::vector<int> getDailyNhifProcedures(const Date& date, long long excludeAmblistRowid);

};

