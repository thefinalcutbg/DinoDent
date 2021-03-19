#include "ManipulationTemplateParser.h"

std::vector<ManipulationTemplate> getManipulationTemplate()
{
    std::ifstream ifs("manipulation.json");
    Json::Reader reader;
    Json::Value m;

    reader.parse(ifs, m);

    const Json::Value& manipulation = m["manipulation"];

    std::vector<ManipulationTemplate> manipulationList;
    manipulationList.reserve(manipulation.size());

    for (int i = 0; i < manipulation.size(); i++)
    {
        ManipulationTemplate m;
        m.type = static_cast<ManipulationType>(manipulation[i]["type"].asInt());
        m.code = manipulation[i]["code"].asInt();
        m.name = manipulation[i]["name"].asString();
        m.price = manipulation[i]["price"].asDouble();

        if (!manipulation[i]["default_diag"].isNull())
        {
            m.diagnosis = manipulation[i]["default_diag"].asString();
        }
        if (!manipulation[i]["material"].isNull())
        {
            m.material = manipulation[i]["material"].asString();
        }
        if (!manipulation[i]["duration"].isNull())
        {
            m.duration = manipulation[i]["duration"].asInt();
        }
        


        manipulationList.emplace_back(m);
    }

    return manipulationList;

}
