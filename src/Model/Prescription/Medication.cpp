#include "Medication.h"

#include <vector>
#include <JsonCpp/json.h>

#include "Resources.h"
#include "Database/Database.h"
#include "Database/DbUpdateStatus.h"

bool Medication::initialize()
{
    //initializing forms
    s_medForms.clear();

    Json::Reader reader;
    Json::Value jsonForms;

    reader.parse(Resources::fromPath(":/style/json_medicationForms.json"), jsonForms);
    s_medForms = std::vector<std::string>(jsonForms.size(), std::string());

    for (auto& pair : jsonForms)
    {
        s_medForms[pair["Key"].asInt()] = pair["Value"].asString();
    }
    

    //initializing 
    Db db;

    s_medications.clear();
    s_medNameToIdx.clear();

    db.newStatement("SELECT COUNT(*) FROM numMed");

    while (db.hasRows()) {

        auto size = db.asInt(0);
        s_medications.reserve(size);
        s_medNameToIdx.reserve(size);
    }

    db.newStatement("SELECT rowid, name, form FROM numMed");

    while (db.hasRows())
    {
        int key = db.asInt(0);
        std::string name = db.asString(1);
        int form = db.asInt(2);

        s_medications[key] = std::make_pair(name, form);

        s_medNameToIdx[name] = key;
    }

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
