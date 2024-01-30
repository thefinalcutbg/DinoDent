#include "Ekatte.h"
#include <unordered_map>
#include <unordered_set>
#include <json/json.h>
#include <array>
#include "Resources.h"

struct EkatteData { std::string name; bool isCity{ 0 }; int rhif{ 0 }; int healthRegion{ 0 }; };

const std::array<std::string, 29> regionCode{
    "",
    "BLG",
    "BGS",
    "VAR",
    "VTR",
    "VID",
    "VRC",
    "GAB",
    "DOB",
    "KRZ",
    "KNL",
    "LOV",
    "MON",
    "PAZ",
    "PER",
    "PVN",
    "PDV",
    "RAZ",
    "RSE",
    "SLS",
    "SLV",
    "SML",
    "SOF",
    "SFO",
    "SZR",
    "TGV",
    "HKV",
    "SHU",
    "JAM"
};

std::unordered_map<int, EkatteData> s_idxToData;
std::unordered_map<std::string, int> s_stringToIdx;
std::unordered_set<int> s_nhifUnfav;
std::unordered_map<int, int> s_hr_to_ekatte;

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

        if (value["unfav"].asBool()) { 
            s_nhifUnfav.insert(value["ekatte"].asInt()); 
        }
    }

    cities.clear();

    reader.parse(Resources::fromPath(":/json/json_hrToEkatte.json"), cities);

    s_hr_to_ekatte.reserve(cities.size());

    for (auto& j : cities)
    {
        int key = j["RHIF"].asInt() * 100 + j["HealthRegion"].asInt();
        int value = j["EKATTE"].asInt();

        s_hr_to_ekatte[key] = value;
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

Ekatte::Ekatte(int rhif, int healthRegion)
{
    int key = rhif * 100 + healthRegion;

    if (!s_hr_to_ekatte.count(key)) return;

    *this = Ekatte(s_hr_to_ekatte[key]);

}

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

bool Ekatte::isValid() const
{
    return ekatteIdx && s_idxToData.count(ekatteIdx);
}

bool Ekatte::isUnfav() const
{
    return s_nhifUnfav.count(ekatteIdx);
}

const std::string& Ekatte::getRegionCode() const
{
    if (!ekatteIdx) return regionCode[0];

    return regionCode[rhif];
}

const std::unordered_map<std::string, int>& Ekatte::cityNameToIdx() { return s_stringToIdx; }
