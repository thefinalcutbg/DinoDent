#pragma once
#include <string>

struct ToothIndex {

    int  index{ -1 };        //-1 for general/several teeth, any in range 0-31 for specific tooth
    bool temp{ false };
    bool supernumeral{ false };


    bool inline isValid() const { return index > -1 && index < 32; }
    std::string getNhifNumenclature() const;

    bool operator == (const ToothIndex& other) const
    {
        if (!isValid()) return isValid() == other.isValid();

        return 
            index == other.index &&
            temp == other.temp &&
            supernumeral == other.supernumeral;
    }
};

template<>
struct std::hash<ToothIndex>
{
    std::size_t operator()(ToothIndex const& i) const noexcept
    {
        return i.index ^ (int(i.temp) << 31) ^ (int(i.supernumeral) << 30);
    }

};