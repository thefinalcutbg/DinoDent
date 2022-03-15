#pragma once

#include <unordered_set>
#include <vector>
#include "Model/AmbList.h"

namespace DbAmbList
{

    //returns the id of the inserted amblist
    std::string insertAmbList(const AmbList& ambList, const std::string& patientID); //returns the rowId of the new instered row
    void deleteCurrentSelection(const std::string& ambID);
    void updateAmbList(const AmbList& ambList);

    AmbList getNewAmbSheet(const std::string& patientID);
    AmbList getListData(const std::string& ambID);
    int getNewNumber(int currentYear, bool nzok);
    bool checkExistingAmbNum(int currentYear, int ambNum);
    std::unordered_set<int> getExistingNumbers(int currentYear);
    std::vector<int> getValidYears();
};

