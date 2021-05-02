#pragma once
#include "Model/Manipulation/ManipulationParser.h"
#include "AbstractORM.h"
#include "sqLite/sqlite3.h"
#include <vector>

#include <QDebug>

class DbManipulation : public AbstractORM
{

public:
    std::vector<Manipulation> getManipulations(const std::string& amblist_id);
    void saveManipulations(const std::string& amblist_id, const std::vector<Manipulation>& mList);

};

