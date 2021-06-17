#include "DateValidator.h"

DateValidator::DateValidator() : min{ 1,1,1900 }, max{ 1,1,3000 }
{
}

Date DateValidator::getMin()
{
    return min;
}

bool DateValidator::validateInput(const Date& date)
{
    
    if (date < min)
    {
        _errorMsg = &_minErrorMsg;
        return false;
    }
    else if (date > max)
    {
        _errorMsg = &_maxErrorMsg;
        return false;
    }

    return true;
}

void DateValidator::setMaxErrorMsg(const std::string& errorMsg)
{
    this->_maxErrorMsg = errorMsg;
}

void DateValidator::setMinErrorMsg(const std::string& errorMsg)
{
    this->_minErrorMsg = errorMsg;
}


Date DateValidator::getMax()
{
    return max;
}

void DateValidator::setMaxDate(const Date& maxDate)
{
    max = maxDate;
}

void DateValidator::setMinDate(const Date& minDate)
{
    min = minDate;
}
