#pragma once

#include "DbProcedure.h"
#include <map>
#include "Model/Tooth/ToothParser.h"
#include "Model/AmbListRow.h"
#include "AbstractORM.h"

#include <vector>


class Procedure;
class AmbList;

class DbAmbList : public AbstractORM 
{

    ToothParser tooth_pareser;
    DbProcedure db_procedures;

    std::string getLastStatus(std::string patientID);
    std::vector<Procedure> previousProcedures(std::string patientID);

public:
    DbAmbList();

    void insertAmbList(AmbList& ambList, std::string& patientID);
    void deleteAmbList(const std::string& ambID);
    void updateAmbList(AmbList& ambList);

    void getListData(const std::string& patientID, int currentMonth, int currentYear, AmbList& ambList);
    void getListData(const std::string& ambID, AmbList& ambList);
    int getNewNumber(int currentYear);
    bool checkExistingAmbNum(int currentYear, int ambNum);
    std::map<int, bool> getExistingNumbers(int currentYear);
    std::vector<AmbListRow> getAmbListRows(const Date& from , const Date& to);
    std::vector<int> getValidYears();

};

