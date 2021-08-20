#include "DateValidator.h"

DateValidator::DateValidator() : dateMin{ 1,1,1900 }, dateMax{ 1,1,3000 }
{
}

Date DateValidator::getMin()
{
    return dateMin;
}

bool DateValidator::validateInput(const Date& date)
{
    
    if (date < dateMin)
    {
        _errorMsg = &_minErrorMsg;
        return false;
    }
    else if (date > dateMax)
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
    return dateMax;
}

void DateValidator::setMaxDate(const Date& maxDate)
{
    dateMax = maxDate;
}

void DateValidator::setMinDate(const Date& minDate)
{
    dateMin = minDate;
}
