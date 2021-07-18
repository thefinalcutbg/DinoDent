#pragma once
#include "Model/Procedure/ProcedureParser.h"
#include "AbstractORM.h"
#include "sqLite/sqlite3.h"
#include <vector>
#include "Model/Procedure/MasterNZOK.h"

#include <QDebug>

class DbProcedure : public AbstractORM
{
  
    ProcedureParser procedureParser;
public:
    std::vector<Procedure> getManipulations(const std::string& amblist_id, const Date& amb_date);
    void saveManipulations(const std::string& amblist_id, const std::vector<Procedure>& mList);
    bool procedureExists(int tooth, int procedure, const std::string& patient_id, const Date& date, int year_range, const std::string& excludeAmb_id);
    std::unordered_map<int, int> totalNZOKProcedures(const std::string& patientID, const std::string& excludeAmbId, int ambList_year);
    bool procedureExists(int tooth, int procedure, const std::string& patient_id, const std::string& excludeAmbId);
};

