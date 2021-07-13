#include "CityCode.h"
#include <sstream>
#include <fstream>

typedef std::string HealthRegion, Region, HRIFCode, Muncipanity, CityString;

std::pair<Muncipanity, Region> CityCode::parseCityString(const CityString& cityString)
{
    constexpr int offset = 10;
    bool munciFound = false;
    int munciPos = 0;
    int regionPos = 0;

    Muncipanity muncipanity;
    Region region;

    for (int i = 0; i < cityString.size(); i++)
    {
        if (cityString[i] == ',')
        {
            if (!munciFound)
            {
                munciPos = i;
                munciFound = true;
            }
            else
            {
                regionPos = i;
                break;
            }
        }
    }

    muncipanity = cityString.substr(munciPos + offset, regionPos - munciPos - offset);
    region = cityString.substr(regionPos + offset, cityString.size() - regionPos + offset);

    return std::make_pair(muncipanity, region);
}

void CityCode::initialize_map()
{
    cityMap.clear();

    std::unordered_map<Muncipanity, HealthRegion> munciMap;
    std::unordered_map<Region, HRIFCode> regionMap;

    std::ifstream infile("munciMap.txt");
    std::string line;

    while (std::getline(infile, line))
    {
        HealthRegion code = line.substr(0, 2);
        std::string muncipanity = line.substr(2, line.size() - 2);
        munciMap[muncipanity] = code;
    }
    infile.close();

    infile.open("regionMap.txt");

    while (std::getline(infile, line))
    {
        HRIFCode code = line.substr(0, 2);
        std::string region = line.substr(2, line.size() - 2);
        regionMap[region] = code;
    }
    infile.close();

    infile.open("cities.txt");

    while (std::getline(infile, line))
    {
        auto [muncipanity, region] = parseCityString(line);
        cityMap[line] = std::make_pair(regionMap[region], munciMap[muncipanity]);
    }

    infile.close();
}

CityCode::CityCode()
{
    if (!_init)
    {
        initialize_map();
        _init = true;
    }

}

std::string CityCode::getLabel(const std::string &cityString) const
{
    if (!cityMap.count(cityString)) return "";
   
    auto [hrif, health_region] = getCodes(cityString);

    return "РЗОК №" + hrif +"\tЗдравен район: " + health_region;
}

std::pair<HealthRegion, HRIFCode> CityCode::getCodes(const std::string &cityString) const
{
    if (!cityMap.count(cityString)) 
        return std::make_pair(std::string{ "-1" }, std::string{ "-1" });

    return cityMap[cityString];
}

bool CityCode::validCityString(const std::string& cityString) const
{
    return cityMap.count(cityString);
}

const std::unordered_map<CityString, std::pair<HealthRegion, HRIFCode>>& CityCode::getMap() const
{
    return cityMap;
}
