#pragma once

#include "DbProcedure.h"
#include <map>

#include "AbstractORM.h"

#include <vector>


class Procedure;
class AmbList;

class DbAmbList : public AbstractORM 
{


    DbProcedure db_procedures;

    std::string getLastStatus(std::string patientID);
    std::vector<Procedure> previousProcedures(std::string patientID);

public:
    DbAmbList();

    //amblist gets updated id after insertion

    std::string insertAmbList(const AmbList& ambList, const std::string& patientID); //returns the rowId of the new instered row
    void deleteCurrentSelection(const std::string& ambID);
    void updateAmbList(const AmbList& ambList);

    AmbList getListData(const std::string& patientID, int currentMonth, int currentYear);
    AmbList getListData(const std::string& ambID);
    int getNewNumber(int currentYear);
    bool checkExistingAmbNum(int currentYear, int ambNum);
    std::map<int, bool> getExistingNumbers(int currentYear);
    std::vector<int> getValidYears();
};

