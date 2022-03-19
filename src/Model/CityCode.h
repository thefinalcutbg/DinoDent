#pragma once

#include <unordered_map>
#include <tuple>

typedef std::string RHIF, HealthRegion, CityString;

namespace CityCode
{
	void initialize();
	const std::string getLabel(const std::string& cityString);
	const std::pair<RHIF, HealthRegion> getCodes(const std::string& cityString);
	bool validCityString(const std::string& cityString);
	const std::string& cityFromIndex(int idx);
	int getDbCityIdx(const std::string& cityString);

	const std::vector<const CityString*>& getCitieStrings();
	
};

