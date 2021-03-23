#include "ToothParserB.h"

Json::Value ToothParserB::getToothJson(const Tooth& t)
{
    Json::Value toothJson = Json::Value(Json::arrayValue);


    if (t.temporary.exists())
    {
        toothJson["Temporary"] = true;
    }


    if (!toothJson.empty())
    {
        toothJson["idx"] = t.index;
    }

    return toothJson;
}

std::string ToothParserB::write(const std::array<Tooth, 32>& teeth)
{
    Json::Value teethJson = Json::Value(Json::arrayValue);

    for (Tooth t : teeth)
    {
        Json::Value toothValue = getToothJson(t);

        if (!toothValue.empty())
        {
            teethJson.append(toothValue);
        }
    }

    return writer.write(teethJson);
}

void ToothParserB::parse(const std::string& jsonString, std::array<Tooth, 32>& teeth)
{
    Json::Value teethJson;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(jsonString, teethJson);

    if (!parsingSuccessful) {
        return;
    }

    for (int i = 0; i < teethJson.size(); i++)
    {
        auto& tooth = teeth[teethJson[i]["idx"].asInt()];
        if (teethJson[i].isMember("Temporary"))
        {
            tooth.temporary.set(teethJson[i]["Temporary"].asBool());
        }
    }

}
