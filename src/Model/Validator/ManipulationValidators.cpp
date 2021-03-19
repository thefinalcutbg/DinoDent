#include "ManipulationValidators.h"
#include <qdebug.h>

DateValidator::DateValidator() : min{1,1,1900}, max{1,1,3000}
{
}

void DateValidator::setAmbListDate(const Date& amb_date)
{
    this->min = amb_date;
    this->max = Date{ amb_date.getMaxDayOfMonth(amb_date.month, amb_date.year),
                      amb_date.month,
                      amb_date.year };
}

bool DateValidator::validate(const Date& date)
{

    if (date < min || date > max)
    {
        return false;
    }

    return true;
}

bool DateValidator::validate(const std::string& text)
{
    Date date(text);

    return validate(date);

}


bool BridgeRangeValidator::validate(int begin, int end)
{
    if ((begin < 16) != (end < 16)){
        return false;
    }

    if (begin == end) {
        return false;
    }
    return true;
}

bool EmptyFieldValidator::validate(const std::string& text)
{
    return !text.empty();
}

bool SurfaceValidator::validate(const std::array<bool, 6>& surfaces)
{
    for (bool s : surfaces)
    {
        if (s) return true;
    }
    return false;
}
