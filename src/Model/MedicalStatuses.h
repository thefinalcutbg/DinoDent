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
	std::vector<MedicalStatus> history;
	std::vector<MedicalStatus> condition;
};