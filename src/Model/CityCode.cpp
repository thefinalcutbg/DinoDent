#include "CityCode.h"

#include <fstream>
#include <utility>
#include <algorithm>

#include "Libraries/JsonCpp/json.h"
#include "Model/FreeFunctions.h"
#include <QDebug>

typedef std::string RHIF, HealthRegion , CityString;


struct CityData { int dbIdx, rhif, healthRegion; };

std::unordered_map <CityString, CityData> cityStringToCodes;
std::vector<const CityString*> dbCodeToCityString;

/*
void sortCitiesAndExport()
{

    std::sort(dbCodeToCityString.begin(), dbCodeToCityString.end(),
        [](const CityString* a, const CityString* b) {
            auto dataA = cityStringToCodes[*a];
            auto dataB = cityStringToCodes[*b];

            if (dataA.rhif != dataB.rhif) return dataA.rhif < dataB.rhif;

            if (dataA.healthRegion != dataB.healthRegion) return dataA.healthRegion < dataB.healthRegion;

            for (int i = 0; i < a->size() || i < b->size(); i++)
                if (a->at(i) != b->at(i)) return a->at(i) < b->at(i);


            return true;
        }
    );

    std::ofstream myfile;
    myfile.open("sortedCities.txt");

    auto counter{ 0 };

    for (auto strPtr : dbCodeToCityString)
    {
        std::string tabEq = cityStringToCodes[*strPtr].rhif < 10 ? "\t" : "";

        myfile << "{ \"dbIdx\" : " + std::to_string(counter) + ",\t"
                    "\"rhif\" : "+ std::to_string(cityStringToCodes[*strPtr].rhif) + ",\t"
                    + tabEq +
                    "\"healthRegion\" : " + std::to_string(cityStringToCodes[*strPtr].healthRegion) + ",\t"
                      "\"city\" : \"" + *strPtr + "\"},\n";

        counter++;
    };

}*/

void CityCode::initialize()
{
    cityStringToCodes.clear();

    std::ifstream ifs("data/cities.json");
    Json::Value cities = Json::arrayValue;

    Json::Reader reader;
    reader.parse(ifs, cities);

    int cityCount = cities.size();

    dbCodeToCityString = std::vector<const CityString*>(cityCount);

    cityStringToCodes.reserve(cities.size());


    for (int i = 0; i < cityCount; i++)
    {
        auto& value = cities[i];
        cityStringToCodes[value["city"].asString()] =
            CityData{ 
                        value["dbIdx"].asInt(),
                        value["rhif"].asInt(), 
                        value["healthRegion"].asInt() 
        };
    }

    for (auto& [key, value] : cityStringToCodes)
    {
        dbCodeToCityString[value.dbIdx] = &key;
    }

}


const std::string CityCode::getLabel(const std::string &cityString)
{
    if (!cityStringToCodes.count(cityString)) return "";
   
    auto [rhif, health_region] = getCodes(cityString);

    return u8"РЗОК №" + rhif + u8"\tЗдравен район №" + health_region;
}


//std::pair<RHIF, HealthRegion> error{ "-1", "-1" };

const std::pair<RHIF, HealthRegion> CityCode::getCodes(const std::string &cityString)
{
    if (!cityStringToCodes.count(cityString))
        throw std::invalid_argument("no codes match the city string argument");

    auto& data = cityStringToCodes[cityString];

    std::string rhif = leadZeroes(data.rhif, 2);
    std::string healthRegion = leadZeroes(data.healthRegion, 2);

    return std::make_pair(rhif, healthRegion);
}

bool CityCode::validCityString(const std::string& cityString)
{
    return cityStringToCodes.count(cityString);
}

const std::string& CityCode::cityFromIndex(int idx)
{
    return *dbCodeToCityString[idx];
}

int CityCode::getDbCityIdx(const std::string& cityString)
{
    return cityStringToCodes[cityString].dbIdx;
}

const std::vector<const CityString*>& CityCode::getCitieStrings()
{
    return dbCodeToCityString;
}


