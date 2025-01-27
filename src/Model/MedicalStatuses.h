#pragma once
#include "Model/ICD10.h"

struct MedicalStatus {

	ICD10 diagnosis;
	std::string nrn;

	bool operator ==(const MedicalStatus& other) {

		return diagnosis.code() == other.diagnosis.code()
			&& nrn == other.nrn;
	}
};

struct MedicalStatuses	
{
    std::vector<MedicalStatus> condition;
    std::vector<MedicalStatus> history;

	static std::string toString(const std::vector<MedicalStatus>& list)
	{
		if (list.empty()) return "Не съобщава";

		std::string result;

		for (auto& status : list) {
			result += status.diagnosis.name();
			result += ", ";
		}
		result.pop_back();
		result.pop_back();

		return result;
	}

    static void insertUnique(const std::vector<MedicalStatus>& from, std::vector<MedicalStatus>& to){

        for(auto& status : from){

            if(std::find(to.begin(), to.end(), status)==to.end()){
                to.push_back(status);
            }
        }

    }
};
