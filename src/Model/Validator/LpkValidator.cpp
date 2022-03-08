#include "LpkValidator.h"
#include <unordered_set>
#include <string_view>

const std::unordered_set<std::string_view> firstTwoSymbols
{
    "01","02","03","04","05","06","07","08","09","10","11",
    "12","13","14","15","16","17","18","19","20","21","22",
    "23","24","25","26","27","28"
};

const std::string notMatchingFirstTwoSymbols{ u8"Невалиден УИН" };

bool LpkValidator::validateInput(const std::string& text)
{
    if (!m_notEmptyValidator.validateInput(text))
    {
        this->_errorMsg = &m_notEmptyValidator.getErrorMessage();
        return false;
    }

    if (!m_digitsValidator.validateInput(text))
    {
        this->_errorMsg = &m_digitsValidator.getErrorMessage();
        return false;
    }

    if (!m_minimumLengthValidator.validateInput(text))
    {
        this->_errorMsg = &m_minimumLengthValidator.getErrorMessage();
        return false;
    }

    if (!firstTwoSymbols.count(std::string_view{ text.c_str(), 2 }))
    {
        this->_errorMsg = &notMatchingFirstTwoSymbols;
        return false;
    }

    return true;
    
}
