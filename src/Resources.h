#pragma once
#include <string>

//converts qt resources to std::string
namespace Resources
{
	void initialize();
	const std::string& citiesJson();
	const std::string& nzokDataJson();
	const std::string& defaultPriceListJson();
	const std::string& defaultDiagnosisListJson();
	const std::string& ksmpJson();
};

