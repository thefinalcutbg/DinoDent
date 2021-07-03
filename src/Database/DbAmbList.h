#pragma once
#include "Model/Patient.h"
#include "Model/AmbList.h"
#include "Model/Date.h"
#include "Model/Tooth/ToothParser.h"
#include "Model/Procedure/ProcedureApplier.h"
#include "DbManipulation.h"
#include "AbstractORM.h"
#include <QDebug>

#include <vector>
#include <map>
#include <iostream>


class DbAmbList : public AbstractORM 
{

    ToothParser parser;
    ProcedureApplier m_applier;
    DbManipulation db_manipulation;

    std::string getOlderStatus(std::string patientID);
    std::vector<Procedure> getOlderManipulations(std::string patientID);

public:
    DbAmbList();

    void insertAmbList(AmbList& ambList, std::string& patientID);
    void getListData(const std::string& patientID, int currentMonth, int currentYear, AmbList& ambList);

    int getNewNumber(const int& currentYear);

    bool checkExistingAmbNum(int currentYear, int ambNum);
    std::map<int, bool> getExistingNumbers(int currentYear);
    void updateAmbList(AmbList& ambList);
};

