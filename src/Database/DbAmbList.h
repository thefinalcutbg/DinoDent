#pragma once

#include "DbProcedure.h"
#include "Model/Tooth/ToothParser.h"
#include "Model/Procedure/ProcedureApplier.h"
#include "Model/AmbListRow.h"
#include "AbstractORM.h"

#include <vector>
#include <map>
#include <iostream>

class Procedure;
class AmbList;

class DbAmbList : public AbstractORM 
{

    ToothParser procedureParser;
    ProcedureApplier m_applier;
    DbProcedure db_manipulation;

    std::string getLastStatus(std::string patientID);
    std::vector<Procedure> getOlderManipulations(std::string patientID);

public:
    DbAmbList();

    void insertAmbList(AmbList& ambList, std::string& patientID);
    void deleteAmbList(const std::string& ambID);
    void updateAmbList(AmbList& ambList);

    void getListData(const std::string& patientID, int currentMonth, int currentYear, AmbList& ambList);
    void getListData(const std::string& ambID, AmbList& ambList);
    int getNewNumber(const int& currentYear);
    bool checkExistingAmbNum(int currentYear, int ambNum);
    std::map<int, bool> getExistingNumbers(int currentYear);
    std::vector<AmbListRow> getAmbListRows(int month, int year);
    std::vector<int> getValidYears();

};

