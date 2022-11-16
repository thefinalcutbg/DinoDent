#include "Ekatte.h"
#include <unordered_map>

#include <JsonCpp/json.h>

#include "Resources.h"

struct EkatteData { std::string name; bool isCity{ 0 }; int rhif{ 0 }; int healthRegion{ 0 }; };

std::unordered_map<int, EkatteData> s_idxToData;
std::unordered_map<std::string, int> s_stringToIdx;


void Ekatte::initialize()
{
    s_idxToData.clear();

    Json::Value cities = Json::arrayValue;

    Json::Reader reader;
    reader.parse(Resources::citiesJson(), cities);

    int cityCount = cities.size();

    s_idxToData.reserve(cities.size());

    for (int i = 0; i < cityCount; i++)
    {

        auto& value = cities[i];

        s_idxToData[value["ekatte"].asInt()] =
         EkatteData{
            value["string"].asString(),
            value["city"].asBool(),
            value["rhif"].asInt(),
            value["hregion"].asInt()
                        
        };
        
        s_stringToIdx[value["string"].asString()] = value["ekatte"].asInt();

    }

}

Ekatte::Ekatte(int idx, const EkatteData& data) :
    ekatteIdx(idx),
    isCity(data.isCity),
    rhif(data.rhif),
    healthRegion(data.healthRegion),
    str(&data.name)
{}

bool Ekatte::isValidStr(const std::string& cityName)
{
    return s_stringToIdx.count(cityName);
}

Ekatte::Ekatte(int idx) : Ekatte(idx, s_idxToData[idx])
{}

Ekatte::Ekatte(const std::string& cityString) : Ekatte(s_stringToIdx[cityString])
{}

#include "Model/FreeFunctions.h"

std::string Ekatte::getRhif() const { return  FreeFn::leadZeroes(rhif, 2); }
std::string Ekatte::getHealthRegion() const { return FreeFn::leadZeroes(healthRegion, 2); }


std::string Ekatte::getString(bool prefix) const
{
    if (!str) return{};

    if (!prefix) return *str;

    return isCity ?
        "гр. " + *str
        :
        "с. " + *str;
}

std::string Ekatte::ekatte() const
{
    return FreeFn::leadZeroes(ekatteIdx, 5);
}

const std::unordered_map<std::string, int>& Ekatte::cityNameToIdx() { return s_stringToIdx; }
