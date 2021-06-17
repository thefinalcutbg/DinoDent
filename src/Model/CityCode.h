#pragma once

#include <unordered_map>
#include <tuple>

class CityCode
{
	typedef std::string HealthRegion;
	typedef std::string Region;
	typedef std::string HRIFCode;
	typedef std::string Muncipanity;
	typedef std::string CityString;

	std::pair<Muncipanity, Region> parseCityString(std::string location);

	inline static std::unordered_map <CityString, std::pair<HealthRegion, HRIFCode>> cityMap;
	inline static bool _init{ false };

public:
	CityCode();
	std::string getLabel(const std::string &cityString);
	std::pair<HRIFCode, HealthRegion> getCodes(const std::string &cityString);
	bool validCityString(const std::string& cityString);
	void initialize_map();
};

