#include "ToothUtils.h"

ToothUtils::ToothUtils() : 
    surfaceNames{"Оклузално", "Медиално", "Дистално", "Букално", "Лингвално", "Цервикално"}
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

    //mapping indexes to tooth nomenclature;
    int permaNumbers[32]{ 18, 17, 16, 15, 14, 13, 12, 11,
                    21, 22, 23, 24, 25, 26, 27, 28,
                    38, 37, 36, 35, 34, 33, 32, 31,
                    41, 42, 43, 44, 45, 46, 47, 48 };
    int tempNumbers[32];

    for (int i = 0; i < 32; i++) {
        tempNumbers[i] = permaNumbers[i] + 40;
        permaMap[i] = permaNumbers[i];
        tempMap[i] = tempNumbers[i];
    }
}

ToothType ToothUtils::getToothType(int index)
{
    return toothTypeMap[index];
}

int ToothUtils::getToothNumber(const int& index, const bool& temporary)
{
    if (index < 0 || index > 31) return 0;

    if (temporary) {
        return tempMap[index];
    }
    return permaMap[index];
}

Quadrant ToothUtils::getQuadrant(int index)
{
    return quadrant[index];
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
