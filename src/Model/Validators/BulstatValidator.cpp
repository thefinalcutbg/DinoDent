#include "BulstatValidator.h"

const std::string error{ "Невалиден булстат" };

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

    static int firstSum9[8]   = { 1, 2, 3, 4, 5, 6, 7, 8 };
    static int secondSum9[8]  = { 3, 4, 5, 6, 7, 8, 9, 10 };
    static int firstSum13[4]  = { 2, 7, 3, 5 };
    static int secondSum13[4] = { 4, 9, 5, 7 };

    //9 digit bulstat

    auto calculateDigit9 = [&](int digits[8]) {

        int sum = 0;
        for (int i = 0; i < 8; i++) {
            sum += (digits[i] * firstSum9[i]);
        }

        int remainder = sum % 11;
        if (remainder != 10)  return remainder;

        // remainder = 10
        int secondSum = 0;
        for (int i = 0; i < 8; i++) {
            secondSum += (digits[i] * secondSum9[i]);
        }

        int secondRem = secondSum % 11;
        if (secondRem != 10) {
            return secondRem;
        }
        // secondRemainder = 10
        return 0;
    };

    if (text.size() == 9)
    {
        int digits[9]{ 0 };

        for (int i = 0; i < 9; i++) digits[i] = int(text[i]) - '0';

        return (digits[8] == calculateDigit9(digits));
    }

    //13 digit bulstat

    int digits[13]{ 0 };

    for (int i = 0; i < 13; i++) digits[i] = int(text[i]) - '0';

    if (digits[8] != calculateDigit9(digits)) return false;

    auto calculateDigit13 = [&](int digits[12]) {

        int sum = 0;
        for (int i = 0; i < 4; i++) {
            sum += (digits[i + 8] * firstSum13[i]);
        }

        int remainder = sum % 11;

        if (remainder != 10) return remainder;

        // remainder= 10
        int secondSum = 0;

        for (int i = 0; i < 4; i++) {
            secondSum += (digits[i + 8] * secondSum13[i]);
        }

        int secondRem = secondSum % 11;

        if (secondRem != 10) return secondRem;

        // secondRemainder= 10
        return 0;

    };
    
    return digits[12] == calculateDigit13(digits);

}
