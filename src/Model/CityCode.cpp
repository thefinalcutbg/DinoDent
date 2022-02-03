#include "CityCode.h"

#include <fstream>
#include <utility>
#include "Libraries/JsonCpp/json.h"

typedef std::string RHIF, HealthRegion , CityString;

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

std::unordered_map <CityString, std::pair<RHIF, HealthRegion>> cityStringToCodes;
std::unordered_map<std::pair<RHIF, HealthRegion>, const CityString*, pair_hash> codesToCityString;

void CityCode::initialize()
{
    cityStringToCodes.clear();
    codesToCityString.clear();

    std::ifstream ifs("data/cities.json");
    Json::Value cities = Json::arrayValue;

    Json::Reader reader;
    reader.parse(ifs, cities);

    cityStringToCodes.reserve(cities.size());
    codesToCityString.reserve(cities.size());

    for (auto& value : cities)
    {
        cityStringToCodes[value["city"].asString()] =
            std::make_pair(value["RHIF"].asString(), value["healthRegion"].asString());
    }

    for (auto& [key, value] : cityStringToCodes)
    {
        codesToCityString[value] = &key;
    }
}


const std::string CityCode::getLabel(const std::string &cityString)
{
    if (!cityStringToCodes.count(cityString)) return "";
   
    auto [rhif, health_region] = getCodes(cityString);

    return "РЗОК №" + rhif +"\tЗдравен район: " + health_region;
}

const std::string& CityCode::getCityString(const RHIF& hrif, const HealthRegion& healthRegion)
{
    return *codesToCityString[std::make_pair(hrif, healthRegion)];
}

std::pair<RHIF, HealthRegion> CityCode::getCodes(const std::string &cityString)
{
    if (!cityStringToCodes.count(cityString)) 
        return std::make_pair(std::string{ "-1" }, std::string{ "-1" });

    return cityStringToCodes[cityString];
}

bool CityCode::validCityString(const std::string& cityString)
{
    return cityStringToCodes.count(cityString);
}

const std::unordered_map<CityString, std::pair<RHIF, HealthRegion>>& CityCode::getMap()
{
    return cityStringToCodes;
}

