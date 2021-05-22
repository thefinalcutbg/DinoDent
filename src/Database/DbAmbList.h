#pragma once
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/Date.h"
#include "Model/Tooth/ToothParser.h"
#include "Model/Manipulation/ManipulationApplier.h"
#include "DbManipulation.h"
#include "AbstractORM.h"
#include <QDebug>

#include <vector>
#include <map>
#include <iostream>


class DbAmbList : public AbstractORM 
{

    ToothParser parser;
    ManipulationApplier m_applier;
    DbManipulation db_manipulation;

    std::string getOlderStatus(std::string patientID);
    std::vector<Manipulation> getOlderManipulations(std::string patientID);

public:
    DbAmbList();

    void insertAmbList(AmbList& ambList, std::string& patientID);
    AmbList* getList(std::string patientID, int currentMonth, int currentYear);

    int getNewNumber(const int& currentYear);

    bool checkExistingAmbNum(const int& currentYear, const int& ambNum);
    std::map<int, bool> getExistingNumbers(const int& currentYear);
    void updateAmbList(AmbList& ambList);
};

