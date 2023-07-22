#pragma once

#include <unordered_set>
#include <vector>
#include "Model/Dental/AmbList.h"

namespace DbAmbList
{

    //returns the id of the inserted amblist
    long long insert(const AmbList& ambList, long long patientRowId); //returns the rowId of the new instered row
    void deleteCurrentSelection(const std::string& ambID);
    void update(const AmbList& ambList);

    AmbList getNewAmbSheet(long long patientRowId);
    AmbList getListData(long long ambID);
    
    int getNewNumber(Date ambDate, bool nhif);

    std::unordered_set<int> getExistingNumbers(int currentYear);

    bool suchNumberExists(int year, int ambNum, long long ambRowid);

    std::vector<AmbList> getMonthlyNhifSheets(int month, int year, bool foreigners = false);

    void updateNrn(const std::string& nrn, long long ambRowId);

};

