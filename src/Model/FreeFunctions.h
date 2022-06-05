#pragma once
#include <string>
#include <sstream>
#include <iomanip>

inline std::string formatDouble(const double& price)
{
    bool isDecimal{ true };

    double intpart;

    if (std::modf(price, &intpart) == 0.0)
        isDecimal = false;

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << price;


    return stream.str();
};

inline std::string leadZeroes(long long num, int totalLength)
{
    std::string formated;
    formated.reserve(totalLength);

    std::string number(std::to_string(num));

    for (int i = 0; i < totalLength - number.length(); i++) {
        formated += "0";
    }

    formated += number;

    return formated;
}

inline std::string leadZeroes(const std::string& number, int totalLength) {

    std::string formated;
    formated.reserve(totalLength);

    for (int i = 0; i < totalLength - number.length(); i++) {
        formated += "0";
    }

    formated += number;

    return formated;
}