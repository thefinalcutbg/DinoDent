#include "LpkValidator.h"
#include <set>
#include <string_view>

const std::set<std::string_view> firstTwoSymbols
{
    "01","02","03","04","05","06","07","08","09","10","11",
    "12","13","14","15","16","17","18","19","20","21","22",
    "23","24","25","26","27","28"
};

const std::string errorMsg{ "Невалиден УИН" };

bool LpkValidator::validateInput(const std::string& text)
{

    this->_errorMsg = &errorMsg;
    if (!m_notEmptyValidator.validateInput(text))
    {
        return false;
    }

    if (!m_digitsValidator.validateInput(text))
    {
        return false;
    }

    if (!m_minimumLengthValidator.validateInput(text))
    {
        return false;
    }

    if (!firstTwoSymbols.count(std::string_view{ text.c_str(), 2 }))
    {
        return false;
    }

    return true;
    
}
