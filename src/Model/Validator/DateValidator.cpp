#include "DateValidator.h"

DateValidator::DateValidator() : min{ 1,1,1900 }, max{ 1,1,3000 }
{
}

void DateValidator::setAmbListDate(const Date& amb_date)
{
    this->min = amb_date;
    this->max = Date{ amb_date.getMaxDayOfMonth(amb_date.month, amb_date.year),
                      amb_date.month,
                      amb_date.year };
}

Date DateValidator::getMin()
{
    return min;
}

bool DateValidator::validate(int d, int m, int y)
{
    Date date(d, m, y);

    if (date < min || date > max)
    {
        return false;
    }

    return true;
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
