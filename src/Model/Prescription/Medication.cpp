#include "Medication.h"

#include <vector>
#include <json/json.h>

#include "Resources.h"
#include "GlobalSettings.h"

bool Medication::initialize()
{
    //initializing forms
    s_medForms.clear();

    Json::Reader reader;
    Json::Value jsonForms;

    reader.parse(Resources::fromPath(":/json/json_medicationForms.json"), jsonForms);
    s_medForms = std::vector<std::string>(jsonForms.size(), std::string());

    for (auto& pair : jsonForms)
    {
        s_medForms[pair["Key"].asInt()] = pair["Value"].asString();
    }
    

    s_medications.clear();
    s_medNameToIdx.clear();

    Json::Value numMedJson;
    auto jsonStr = GlobalSettings::getNumMedJson();

    if(jsonStr.empty()) return false;

    if(!reader.parse(jsonStr, numMedJson)) return false;

    s_medications.reserve(numMedJson["meds"].size());
    s_medNameToIdx.reserve(numMedJson["meds"].size());

    for(auto& entity : numMedJson["meds"])
    {
        int key = entity["id"].asInt();
        std::string name = entity["descr"].asString();
        int form = entity["form"].asInt();

        s_medications[key] = std::make_pair(name, form);

        s_medNameToIdx[name] = key;
    }

    s_last_update = numMedJson["last_updated"].asString();

    return s_medications.size();

}

Medication::Medication(int key) :
    key(key), form(s_medications[key].second)
{
}

std::vector<std::string> Medication::dosageList() const {

    std::vector<std::string> result;

    for (auto& d : dosage) {
        result.push_back(d.parse());
    }

    return result;
}



bool Medication::setName(const std::string& name)
{
    if (!s_medNameToIdx.count(name)) return false;

    key = s_medNameToIdx[name];
    form = s_medications[key].second;

    return true;
}

const std::string& Medication::name() const
{
    if (!key) return s_dummyResult;

    return std::get<0>(s_medications[key]);
}

const std::string& Medication::formStr() const
{
    if (!form) return s_dummyResult;

    return s_medForms[form];
}

std::string Medication::parseFullDosage() const
{
    std::string dosageStr;

    for (int i = 0; i < dosage.size(); i++) {

        if (i) {

            dosageStr += " След това ";
        }

        if (dosage[i].frequency == 1) {
            dosageStr += i ? "по " : "По ";
        }

        dosageStr += dosage[i].parse();

        if (dosageStr.back() == ' ') {
            dosageStr.pop_back();
        }

        dosageStr += ".";
    }

    return dosageStr;
}

std::string Medication::quantityParsed() const
{
    std::string result = std::to_string(quantity);

    result += " ";

    if (byForm) {

        result += quantity > 1 ? "лекарствени форми" : "лекарствена форма";
    }
    else {
        result += quantity > 1 ? "опаковки" : "опаковка";
    }

    return result;
}

bool Medication::isValidName(const std::string& name)
{
    return s_medNameToIdx.count(name);
}

const std::unordered_map<std::string, int>& Medication::names()
{
    return s_medNameToIdx;
}

const std::string& Medication::getFormByKey(int key)
{
    if(key >= 0 && key < s_medForms.size()) return s_medForms[key];

    return s_dummyResult;
}

const std::vector<std::string>& Medication::forms() {
    return s_medForms;
}
