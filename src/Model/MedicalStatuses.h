#pragma once
#include "Model/ICD10.h"

struct MedicalStatuses	
{
    std::vector<ICD10> condition;
    std::vector<ICD10> history;

	static std::string toString(const std::vector<ICD10>& list)
	{
		if (list.empty()) return "Не съобщава";

		std::string result;

		for (auto& status : list) {
			result += status.name();
			result += ", ";
		}
		result.pop_back();
		result.pop_back();

		return result;
	}

    static void insertUnique(const std::vector<ICD10>& from, std::vector<ICD10>& to){

        for(auto& icd : from){

            if(std::find(to.begin(), to.end(), icd)==to.end()){
                to.push_back(icd);
            }
        }

    }
};
