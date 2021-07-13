#pragma once
#include "Model/Procedure/ProcedureParser.h"
#include "AbstractORM.h"
#include "sqLite/sqlite3.h"
#include <vector>
#include "Model/Procedure/MasterNZOK.h"

#include <QDebug>

class DbManipulation : public AbstractORM
{
  
    ProcedureParser procedureParser;
public:
    std::vector<Procedure> getManipulations(const std::string& amblist_id, const Date& amb_date);
    void saveManipulations(const std::string& amblist_id, const std::vector<Procedure>& mList);

};

