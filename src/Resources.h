#pragma once
#include <string>
#include <vector>
/*
Ideally these should be used only once in the program lifetime
*/
namespace Resources
{
	std::string fromPath(const char* path);
	std::string citiesJson();
	std::string nhifDataJson();
	std::string defaultPriceListJson();
	std::string defaultDiagnosisListJson();
	std::string icdJson();
	std::vector<std::string> dbSchema();
	std::vector<std::string> getMigrationScript(int version);
};

