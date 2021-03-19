#pragma once

#include <sstream>
#include <fstream>
#include <unordered_map>
#include <tuple>
#include "Model/Validator/PatientValidators.h"

class CityCode
{

	std::unordered_map<std::string, std::string> munciMap;
	std::unordered_map<std::string, std::string> regionMap;
	CityValidator validator;

public:
	CityCode();
	std::string getLabel(const std::string &cityString);
	std::pair<std::string, std::string> getHealthRegionAndHRIFCode(const std::string &cityString);
};

