#pragma once
#include <string>
#include <sstream>
#include <iomanip>

std::string formatDouble(const double& price)
{
    bool isDecimal{ true };

    double intpart;

    if (std::modf(price, &intpart) == 0.0)
        isDecimal = false;

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << price;


    return stream.str();
};