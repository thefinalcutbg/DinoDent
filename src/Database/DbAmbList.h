#pragma once

#include <unordered_set>
#include <vector>
#include "Model/AmbList.h"

namespace DbAmbList
{

    //returns the id of the inserted amblist
    long long insert(const AmbList& ambList, long long patientRowId); //returns the rowId of the new instered row
    void deleteCurrentSelection(const std::string& ambID);
    void update(const AmbList& ambList);

    AmbList getNewAmbSheet(long long patientRowId);
    AmbList getListData(long long ambID);
    
    int getNewNumber(Date ambDate, bool nzok);
    bool checkExistingAmbNum(int currentYear, int ambNum);
    std::unordered_set<int> getExistingNumbers(int currentYear);

    //Gets all NHIF lists for report generation:
    std::vector<long long> getRowIdNhif(int month, int year);
    //with HIS this function becomes obsolete. Use getListData() instead.
    AmbList getListNhifProceduresOnly(long long rowid);

};

