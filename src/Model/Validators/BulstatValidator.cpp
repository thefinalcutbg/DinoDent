#include "BulstatValidator.h"

const std::string error{ u8"Невалиден булстат" };

bool BulstatValidator::validateInput(const std::string& text)
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

    this->_errorMsg = &error;

    if (text.size() != 9 && text.size() != 13){
        return false;
    }

    int length = text.size();

    constexpr int weigths[12]{ 1,2,3,4,5,6,7,8,9,10,11,12};
    constexpr int weigths2[12]{ 3,4,5,6,7,8,9,10,4,9,5,7 };

    



    return true;
}
