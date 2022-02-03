#pragma once

#include <unordered_map>
#include <tuple>

typedef std::string RHIF, HealthRegion, CityString;

namespace CityCode
{
	void initialize();
	const std::string getLabel(const std::string& cityString);
	const std::string& getCityString(const RHIF& hrif, const HealthRegion& healthRegion);
	std::pair<HealthRegion, RHIF> getCodes(const std::string& cityString);
	bool validCityString(const std::string& cityString);

	const std::unordered_map <CityString, std::pair<RHIF, HealthRegion>>& getMap();
	
};

