#pragma once
#include <string>
#include <vector>

struct MedicalStatuses	
{
	std::vector<std::string> condition;
	std::vector<std::string> history;

	static std::string toString(const std::vector<std::string>& list)
	{
		if (list.empty()) return "Не съобщава";

		std::string result;

		for (auto status : list) {
			result += status;
			result += ", ";
		}
		result.pop_back();
		result.pop_back();

		return result;
	}
};