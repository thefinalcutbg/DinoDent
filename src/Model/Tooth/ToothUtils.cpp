#include "ToothUtils.h"
#include <string>
#include "Tooth.h"

std::array<std::string, 6> ToothUtils::surfaceNames
{ "Оклузално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално" };


ToothUtils::ToothUtils()
{
}

ToothType ToothUtils::getToothType(int index)
{
    return static_cast<ToothType>(types[index]);
}

int ToothUtils::getToothNumber(int index, bool temporary)
{
    if (index < 0 || index > 31) return -1;

    if (temporary) {
        return numbers[index]+40;
    }
    return numbers[index];
}

std::string ToothUtils::getNomenclature(int tooth_idx, bool temporary) 
{ 
    auto num = getToothNumber(tooth_idx, temporary);
    if (num == -1)
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
            "Куспидално",
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
                                            "Инцизално",
                                             surfaceNames[1],
                                             surfaceNames[2],
                                             surfaceNames[3],
                                             surfaceNames[4],
                                             surfaceNames[5]
        };
    }

    return surfaceNames; //any other teeth
    
}
