﻿#include "ToothUtils.h"
#include <string>
#include "Tooth.h"

constexpr std::array<int, 32> numbers
{
  18, 17, 16, 15, 14, 13, 12, 11,     21, 22, 23, 24, 25, 26, 27, 28,
  38, 37, 36, 35, 34, 33, 32, 31,     41, 42, 43, 44, 45, 46, 47, 48
};

constexpr std::array<int, 32> procedureType
{
    0,0,0,1,1,2,2,2,    2,2,2,1,1,0,0,0,
    0,0,0,1,1,2,2,2,    2,2,2,1,1,0,0,0
};


std::array<std::string, 6> surfaceNames
{ u8"Оклузално", u8"Медиално", u8"Дистално", u8"Букално", u8"Лингвално", u8"Цервикално" };



ToothType ToothUtils::getToothType(int index)
{
    return static_cast<ToothType>(procedureType[index]);
}

constexpr  int invalidToothIdx = 99;

int ToothUtils::getToothNumber(int index, bool temporary)
{
    if (index < 0 || index > 31) return invalidToothIdx;

    if (temporary) {
        return numbers[index]+40;
    }
    return numbers[index];
}

std::string ToothUtils::getNomenclature(int tooth_idx, bool temporary) 
{ 
    auto num = getToothNumber(tooth_idx, temporary);
    if (num == invalidToothIdx)
        return std::string();

    return std::to_string(num);
}
std::string ToothUtils::getNomenclature(const Tooth& t) { return getNomenclature(t.index, t.temporary.exists());}

Quadrant ToothUtils::getQuadrant(int index)
{ 
    if (index < 8) return Quadrant::First;
    if (index < 16) return Quadrant::Second;
    if (index < 24) return Quadrant::Third;

    return Quadrant::Fourth;

}

std::array<std::string, 6> ToothUtils::getSurfaceNames(int index)
{

    if (index == 5 || index == 10 || index == 21 || index == 26) //canine teeth
    {
        return std::array<std::string, 6>{
                u8"Куспидално",
                surfaceNames[1],
                surfaceNames[2],
                surfaceNames[3],
                surfaceNames[4],
                surfaceNames[5]
        };
    }

    if (getToothType(index) == ToothType::Frontal) //frontal teeth
    {
        return std::array<std::string, 6>{
                                            u8"Инцизално",
                                             surfaceNames[1],
                                             surfaceNames[2],
                                             surfaceNames[3],
                                             surfaceNames[4],
                                             surfaceNames[5]
        };
    }

    return surfaceNames; //any other teeth
    
}

std::pair<int, bool> ToothUtils::getArrayIdxAndTemp(int toothNum)
{
    if (toothNum == 99)
        return { -1, false };

    bool temp = toothNum > 48;

    if (temp) {
        toothNum = toothNum - 40;
    }

    for (int i = 0; i < numbers.size(); i++) {
        
        if (toothNum == numbers[i]) {
            return { i, temp };
        }
    }

    return { -1, false };

}

std::string ToothUtils::getName(int idx, bool temp)
{
    static constexpr const char* toothPerm[32]
    {
        u8"Трети горен десен молар",
        u8"Втори горен десен молар",
        u8"Първи горен десен молар",
        u8"Втори горен десен премолар",
        u8"Първи горен десен премолар",
        u8"Горен десен кучешки",
        u8"Горен десен латерален резец",
        u8"Горен десен централен резец",
        u8"Горен ляв централен резец",
        u8"Горен ляв латерален резец",
        u8"Горен ляв кучешки",
        u8"Първи горен ляв премолар",
        u8"Втори горен ляв премолар",
        u8"Първи горен ляв молар",
        u8"Втори горен ляв молар",
        u8"Трети горен ляв молар",

        u8"Трети долен ляв молар",
        u8"Втори долен ляв молар",
        u8"Първи долен ляв молар",
        u8"Втори долен ляв премолар",
        u8"Първи долен ляв премолар",
        u8"Долен ляв кучешки",
        u8"Долен ляв латерален резец",
        u8"Долен ляв централен резец",
        u8"Долен десен централен резец",
        u8"Долен десен латерален резец",
        u8"Долен десен кучешки",
        u8"Първи долен десен премолар",
        u8"Втори долен десен премолар",
        u8"Първи долен десен молар",
        u8"Втори долен десен молар",
        u8"Трети долен десен молар",
    };

    static constexpr const char* toothTemp[32]
    {
        "","","",
        u8"Втори горен десен временен молар",
        u8"Първи горен десен временен молар",
        u8"Горен десен временен кучешки",
        u8"Втори горен десен временен резец",
        u8"Първи горен десен временен резец",
        u8"Първи горен ляв временен резец",
        u8"Втори горен ляв временен резец",
        u8"Горен ляв временен кучешки",
        u8"Първи горен ляв временен молар",
        u8"Втори горен ляв временен молар",
        "","","",

        "","","",
        u8"Втори долен ляв временен молар",
        u8"Първи долен ляв временен молар",
        u8"Долен ляв временен кучешки",
        u8"Втори долен ляв временен резец",
        u8"Първи долен ляв временен резец",
        u8"Първи долен десен временен резец",
        u8"Втори долен десен временен резец",
        u8"Долен десен временен кучешки",
        u8"Първи долен десен временен молар",
        u8"Втори долен десен временен молар",
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