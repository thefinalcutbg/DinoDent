#include "ToothUtils.h"
#include <string>
#include "Tooth.h"

constexpr std::array<int, 32> s_tooth_FDI
{
  18, 17, 16, 15, 14, 13, 12, 11,     21, 22, 23, 24, 25, 26, 27, 28,
  38, 37, 36, 35, 34, 33, 32, 31,     41, 42, 43, 44, 45, 46, 47, 48
};

constexpr  int nhifNoTooth = 99;


ToothType ToothUtils::getToothType(int index)
{

    constexpr static std::array<int, 32> toothType
    {
        0,0,0,1,1,2,2,2,    2,2,2,1,1,0,0,0,
        0,0,0,1,1,2,2,2,    2,2,2,1,1,0,0,0
    };

    return static_cast<ToothType>(toothType[index]);
}



int ToothUtils::getToothNumber(int index, bool temporary)
{
    if (index < 0 || index > 31) return nhifNoTooth;

    if (temporary) {
        return s_tooth_FDI[index]+40;
    }
    return s_tooth_FDI[index];
}

std::string ToothUtils::getNomenclature(int tooth_idx, bool temporary) 
{ 
    auto num = getToothNumber(tooth_idx, temporary);
    if (num == nhifNoTooth)
        return std::string();

    return std::to_string(num);
}
std::string ToothUtils::getNomenclature(const Tooth& t) { 
    return getNomenclature(t.index, t.temporary);
}

std::string ToothUtils::getNhifNumber(int index, bool temporary, bool hyperdontic)
{
    int toothNumber = getToothNumber(index, temporary);

    if (toothNumber == nhifNoTooth) return "99";

    if (!hyperdontic) return std::to_string(toothNumber);

    std::string result = std::to_string(getToothNumber(index, false));

    static constexpr std::array<char, 4> DsnQuadrant = {'A','B','C','D'};

    result[0] = DsnQuadrant.at(static_cast<int>(getQuadrant(index)));

    return result;
}

Quadrant ToothUtils::getQuadrant(int index)
{ 
    if (index < 8) return Quadrant::First;
    if (index < 16) return Quadrant::Second;
    if (index < 24) return Quadrant::Third;

    return Quadrant::Fourth;

}

std::array<std::string, 6> ToothUtils::getSurfaceNames(int index)
{

    auto getOcclusalName = [&] {

        //canine
        if (index == 5 || index == 10 || index == 21 || index == 26) return "Куспидално";

        //incisor
        if (getToothType(index) == ToothType::Frontal) return "Инцизално";

        //molar and premolar
        return "Оклузално";
    };

    auto getBuccalName = [&] {
        if (getToothType(index) == ToothType::Frontal) return "Лабиално";

        return "Букално";

    };

    auto getLingualName = [&] {

        if (index < 16) return "Палатинално";

        return "Лингвално";
    };

    
    return std::array<std::string, 6>{
            getOcclusalName(),
            "Медиално",
            "Дистално",
            getBuccalName(),
            getLingualName(),
            "Цервикално"
    };

    
}

ToothUtils::ToothProcedureCode ToothUtils::getToothFromNhifNum(const std::string& toothNhif)
{
    if (toothNhif.length() != 2) return { -1, false, false };

    if (toothNhif == "99") return { -1, false, false };

    int toothNum = -1;

    bool hyperdontic = false;

    if (!std::isdigit(toothNhif[0]))
    {
        std::string val;

        val += char(int(toothNhif[0]) - 16);
        val += toothNhif[1];

        toothNum = std::stoi(val);

        hyperdontic = true;
    }
    else
    {
        toothNum = std::stoi(toothNhif);
    }

    bool temp = toothNum > 48;

    if (temp) {
        toothNum = toothNum - 40;
    }

    for (int i = 0; i < s_tooth_FDI.size(); i++) {
        
        if (toothNum == s_tooth_FDI[i]) {
            return { i, temp, hyperdontic };
        }
    }

    return { -1, temp, hyperdontic };

}

std::string ToothUtils::getName(int idx, bool temp)
{
    static constexpr const char* toothPerm[32]
    {
        "Трети горен десен молар",
        "Втори горен десен молар",
        "Първи горен десен молар",
        "Втори горен десен премолар",
        "Първи горен десен премолар",
        "Горен десен кучешки",
        "Горен десен латерален резец",
        "Горен десен централен резец",
        "Горен ляв централен резец",
        "Горен ляв латерален резец",
        "Горен ляв кучешки",
        "Първи горен ляв премолар",
        "Втори горен ляв премолар",
        "Първи горен ляв молар",
        "Втори горен ляв молар",
        "Трети горен ляв молар",

        "Трети долен ляв молар",
        "Втори долен ляв молар",
        "Първи долен ляв молар",
        "Втори долен ляв премолар",
        "Първи долен ляв премолар",
        "Долен ляв кучешки",
        "Долен ляв латерален резец",
        "Долен ляв централен резец",
        "Долен десен централен резец",
        "Долен десен латерален резец",
        "Долен десен кучешки",
        "Първи долен десен премолар",
        "Втори долен десен премолар",
        "Първи долен десен молар",
        "Втори долен десен молар",
        "Трети долен десен молар",
    };

    static constexpr const char* toothTemp[32]
    {
        "","","",
        "Втори горен десен временен молар",
        "Първи горен десен временен молар",
        "Горен десен временен кучешки",
        "Втори горен десен временен резец",
        "Първи горен десен временен резец",
        "Първи горен ляв временен резец",
        "Втори горен ляв временен резец",
        "Горен ляв временен кучешки",
        "Първи горен ляв временен молар",
        "Втори горен ляв временен молар",
        "","","",

        "","","",
        "Втори долен ляв временен молар",
        "Първи долен ляв временен молар",
        "Долен ляв временен кучешки",
        "Втори долен ляв временен резец",
        "Първи долен ляв временен резец",
        "Първи долен десен временен резец",
        "Втори долен десен временен резец",
        "Долен десен временен кучешки",
        "Първи долен десен временен молар",
        "Втори долен десен временен молар",
        "","",""
    };

    if (!(idx < 0) && (idx < 32)) {
        return temp ?
            toothTemp[idx]
            :
            toothPerm[idx];
    }

    return {};


}

ToothUtils::ToothProcedureCode ToothUtils::getToothFromHisNum(const std::string& toothNum, bool hyperdontic)
{
    auto result = getToothFromNhifNum(toothNum);

    result.hyperdontic = hyperdontic;

    return result;
    
}

const std::array<int, 32>& ToothUtils::FDINumbers()
{
    return s_tooth_FDI;
}
