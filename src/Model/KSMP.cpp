#include "KSMP.h"


#include <unordered_map>
#include <tuple>
#include <array>
#include <JsonCpp/json.h>

#include "Resources.h"

constexpr int procedureTypeCount = 11;

std::unordered_map<std::string, KSMP> codeToInstance;
std::unordered_map<int, std::string> blocks;
std::unordered_map<int, std::string > chapters;
std::array<std::vector<const KSMP*>, procedureTypeCount> ksmpByType;

void KSMP::initialize()
{

    Json::Value ksmpList = Json::arrayValue;

    Json::Reader reader;
    reader.parse(Resources::ksmpJson(), ksmpList);

    codeToInstance.reserve(ksmpList.size());
   
    for (auto& value : ksmpList)
    {

       auto code = value["code"].asString();
       auto block = value["block"].asInt();
       auto chapter = value["chapter"].asInt();
       
       if (!blocks.count(block)) { blocks[block] = value["blockname"].asString(); }
       if (!chapters.count(chapter)) { chapters[chapter] = value["chaptername"].asString(); }

       codeToInstance[code] = 
           KSMP{
                code,
                value["name"].asString(),
                block,
                chapter,
                value["type"].asInt()
           };


       ksmpByType[value["type"].asInt()].push_back(&codeToInstance[code]);

    }

}


const KSMP& KSMP::getByCode(const std::string& code)
{
    return codeToInstance[code];
}

const std::string& KSMP::getName(const std::string& code)
{
    return codeToInstance[code].name;
}

const std::string& KSMP::chapterName(int chapter)
{
    return chapters[chapter];
}

const std::string& KSMP::blockName(int block)
{
    return blocks[block];
}

#include "Model/Procedure/Procedure.h"

const std::vector<const KSMP*>& KSMP::getByType(ProcedureType type)
{

    return ksmpByType[static_cast<int>(type)];

}


const std::vector<const KSMP*>& KSMP::getByType(ProcedureTemplateType type)
{
    //fixing the offset
    constexpr ProcedureType templToProcedureType[9]{
        ProcedureType::general,
        ProcedureType::any,
        ProcedureType::obturation,
        ProcedureType::extraction,
        ProcedureType::endo,
        ProcedureType::crown,
        ProcedureType::implant,
        ProcedureType::fibersplint,
        ProcedureType::removecrown
    };

    return getByType(templToProcedureType[static_cast<int>(type)]);

}