#include "FreeFunctions.h"
#include <sstream>
#include <iomanip>
#include <fstream>

std::string FreeFn::formatDouble(const double& price)
{
    bool isDecimal{ true };

    double intpart;

    if (std::modf(price, &intpart) == 0.0)
        isDecimal = false;

    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << price;


    return stream.str();
}

std::string FreeFn::leadZeroes(long long num, int totalLength)
{
    std::string formated;
    formated.reserve(totalLength);
    std::string number(std::to_string(num));

    if (totalLength < number.length()) return number;

    for (int i = 0; i < totalLength - number.length(); i++) {
        formated += "0";
    }

    formated += number;

    return formated;
}

std::string FreeFn::leadZeroes(const std::string& number, int totalLength)
{

    std::string formated;
    formated.reserve(totalLength);

    if (totalLength < number.length()) return number;

    for (int i = 0; i < totalLength - number.length(); i++) {
        formated += "0";
    }

    return formated + number;
}

std::string FreeFn::getFile(const std::string& filepath)
{

    std::ifstream t(filepath);
    return std::string((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());

}

std::string FreeFn::removeTrailing(double a)
{
    auto result = std::to_string(a);

    while (result.back() == '0' && result.size()) result.pop_back();

    if (result.back() == '.') result.pop_back();

    return result;
}

#include <QUuid>

std::string FreeFn::getUuid()
{
    return QUuid::createUuid().toString(QUuid::StringFormat::Id128).toStdString();
}
