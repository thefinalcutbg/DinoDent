#include "ToothUtils.h"
#include <string>
#include "Tooth.h"

std::array<std::string, 6> ToothUtils::surfaceNames
{ "Оклузално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално" };


ToothUtils::ToothUtils()
{

  

    for (int i = 0; i < 32; i++) //mapping indexes to tooth types
    {
        if (i == 3 || i == 4 || i == 11 || i == 12)
            toothTypeMap[i] = ToothType::Premolar;
        else if (i == 19 || i == 20 || i == 27 || i == 28)
            toothTypeMap[i] = ToothType::Premolar;
        else if ((i > 2 && i < 13) || (i > 18 && i < 29))
            toothTypeMap[i] = ToothType::Frontal;
        else toothTypeMap[i] = ToothType::Molar;
    }


    for (int i = 0; i < 32; i++) //mapping indexes to quadrants
    {
        if (i < 8) quadrant[i] = Quadrant::First;
        else if (i < 16) quadrant[i] = Quadrant::Second;
        else if (i < 24) quadrant[i] = Quadrant::Third;
        else quadrant[i] = Quadrant::Fourth;
    }


}

ToothType ToothUtils::getToothType(int index)
{
    return toothTypeMap[index];
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

Quadrant ToothUtils::getQuadrant(int index) { return quadrant[index]; }

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

    if (toothTypeMap[index] == ToothType::Frontal) //frontal teeth
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
