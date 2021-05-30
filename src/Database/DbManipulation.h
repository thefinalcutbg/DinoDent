#pragma once
#include "Model/Manipulation/ManipulationParser.h"
#include "AbstractORM.h"
#include "sqLite/sqlite3.h"
#include <vector>
#include "Model/Manipulation/MasterNZOK.h"

#include <QDebug>

class DbManipulation : public AbstractORM
{
  
    ManipulationParser parser;
public:
    std::vector<Manipulation> getManipulations(const std::string& amblist_id, const Date& amb_date);
    void saveManipulations(const std::string& amblist_id, const std::vector<Manipulation>& mList);

};

