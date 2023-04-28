#pragma once
#include <string>
#include <vector>

struct MedicalStatus 
{
	std::string nrn;
	std::string data;
};

struct MedicalStatuses	
{
	std::vector<MedicalStatus> allergies;
	std::vector<MedicalStatus> condition;
	std::vector<MedicalStatus> history;

	static std::string getAsString(const std::vector<MedicalStatus>& list)
	{
		if (list.empty()) return "Не съобщава";

		std::string result;

		for (auto status : list) {
			result +=(status.data);
			result += ", ";
		}
		result.pop_back();
		result.pop_back();

		return result;
	}
};