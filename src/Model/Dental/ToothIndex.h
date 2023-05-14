#pragma once
#include <string>

struct ToothIndex {

    int  index{ -1 };        //-1 for general/several teeth, any in range 0-31 for specific tooth
    bool temp{ false };
    bool supernumeral{ false };


    bool inline isValid() const { return index > -1 && index < 32; }
    std::string getNhifNumenclature() const;
};
