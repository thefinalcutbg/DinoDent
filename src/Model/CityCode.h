#pragma once

#include <unordered_map>
#include <tuple>

class CityCode
{
	typedef std::string HealthRegion, Region, HRIFCode, Muncipanity, CityString;

	std::pair<Muncipanity, Region> parseCityString(CityString location);

	inline static std::unordered_map <CityString, std::pair<HealthRegion, HRIFCode>> cityMap;
	inline static bool _init{ false };

public:
	CityCode();
	std::string getLabel(const std::string &cityString) const;
	std::pair<HRIFCode, HealthRegion> getCodes(const std::string &cityString) const;
	bool validCityString(const std::string& cityString) const;
	const std::unordered_map <CityString, std::pair<HealthRegion, HRIFCode>>& getMap() const;
	void initialize_map();
};

