#include "CityCode.h"

CityCode::CityCode()
{
    std::ifstream infile("munciMap.txt");
    std::string line;

    while (std::getline(infile, line))
    {
        std::string code = line.substr(0, 2);
        std::string muncipanity = line.substr(2, line.size() - 2);
        munciMap[muncipanity] = code;
    }
    infile.close();
    
    infile.open("regionMap.txt");
    
    while (std::getline(infile, line))
    {
        std::string code = line.substr(0, 2);
        std::string region = line.substr(2, line.size() - 2);
        regionMap[region] = code;
    }
    infile.close();
}

std::string CityCode::getLabel(const std::string &cityString)
{
    if(!validator.validate(cityString))
    {
        return "";
    }
        
    auto codes = getHealthRegionAndHRIFCode(cityString);
    return "РЗОК №" + std::get<0>(codes)
        + "\tЗдравен район: " + std::get<1>(codes);
}

std::pair<std::string, std::string> CityCode::getHealthRegionAndHRIFCode(const std::string &cityString)
{
    std::string textField = cityString;
    std::string healthRegionCode;
    std::string RHIFCode;

    bool munciFound = false;
    int munciPos = 0;
    int regionPos = 0;

    for (int i = 0; i < textField.size(); i++)
    {
        if (textField[i] == ',')
        {
            if (!munciFound)
            {
                munciPos = i;
                munciFound = true;
            }
            else
            {
                regionPos = i;
                break;
            }
        }
    }

    healthRegionCode = textField.substr(munciPos + 10, regionPos - munciPos - 10);
    RHIFCode = textField.substr(regionPos + 10, textField.size() - regionPos + 10);

    return std::pair<std::string, std::string>(regionMap[RHIFCode], munciMap[healthRegionCode]);
}
