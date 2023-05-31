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

#include <QDateTime>

std::string FreeFn::getTimeStamp()
{
    return QDateTime::currentDateTime().toString(Qt::ISODate).toStdString();
}

std::string FreeFn::escapeXml(const std::string& data)
{
    std::string result;
    result.reserve(data.size());

    for (size_t pos = 0; pos != data.size(); ++pos) {
        switch (data[pos]) {
        case '&':  result.append("&amp;");       break;
        case '\"': result.append("&quot;");      break;
        case '\'': result.append("&apos;");      break;
        case '<':  result.append("&lt;");        break;
        case '>':  result.append("&gt;");        break;
        default:   result.append(&data[pos], 1); break;
        }
    }
    
    return result;
}

bool FreeFn::contains(const std::string& parent, const std::string& child)
{
    return QString(parent.c_str()).contains(child.c_str());
}
