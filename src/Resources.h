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
	std::string nzokDataJson();
	std::string defaultPriceListJson();
	std::string defaultDiagnosisListJson();
	std::string ksmpJson();
	std::string mkbJson();
	std::vector<std::string> dbSchema();
};
