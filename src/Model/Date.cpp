#include "Date.h"

#include <QDate>

int Date::monthDays[12]{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

Date::Date() :
    day(1),
    month(1),
    year(1900)
{}

Date::Date(int day, int month, int year) :
    day(day),
    month(month),
    year(year)
{}

Date::Date(const std::string& dd_dot_MM_dot_yyyy):
    day(stoi(dd_dot_MM_dot_yyyy.substr(0, 2))),
    month(stoi(dd_dot_MM_dot_yyyy.substr(3, 2))),
    year(stoi(dd_dot_MM_dot_yyyy.substr(6, 4)))
{}


bool Date::isLeapYear(int year)
{
    if (year % 4 == 0)
    {
        if (year % 100 == 0)
        {
            if (year % 400 == 0) { return true; }
            else { return false; }
        }
        else { return true; }
    }
    else return false;
}

int Date::getMaxDayOfMonth() const
{
    if (month < 1 || month > 12) throw "invalid month!";

    if (month != 2)
    {
        return monthDays[month - 1];
    }

    if (isLeapYear(year)) {
        return monthDays[1] + 1;
    }

    return monthDays[1];

}

Date Date::yesterday() const
{
    if (day != 1) return Date{ day - 1, month, year };

    if (month != 1) return Date{getMaxDayOfMonth(), month-1, year};

    return Date{31, 12, year - 1};
}

Date Date::getBirthdateFromEgn(const std::string& egn)
{

    int year = stoi(egn.substr(0, 2));
    int month = stoi(egn.substr(2, 2));
    int day = stoi(egn.substr(4, 2));

    //converting year and month to full numbers:
    if (month > 20 && month < 33) { year = year + 1800; month = month - 20; }
    else if (month <= 12) { year = year + 1900; }
    else if (month >= 41) { year = year + 2000; month = month - 40; }


    return Date(day, month, year);
}


std::string Date::toString(bool suffix) const
{

      std::string dayStr;

    if (day < 10)
        dayStr = "0" + std::to_string(day);
    else
        dayStr = std::to_string(day);

    std::string monthStr;

    if (month < 10)
        monthStr = "0" + std::to_string(month);
    else
        monthStr = std::to_string(month);

    std::string yearStr = std::to_string(year);

    if (suffix)
        yearStr += u8" г.";

    return dayStr + "." + monthStr + "." + yearStr;
}

std::string Date::toXMLString() const
{
    std::string dayStr;

    if (day < 10)
        dayStr = "0" + std::to_string(day);
    else
        dayStr = std::to_string(day);

    std::string monthStr;

    if (month < 10)
        monthStr = "0" + std::to_string(month);
    else
        monthStr = std::to_string(month);

    return  std::to_string(year) + "-" + monthStr + "-" + dayStr;
}

std::string Date::toXMLReportFileName() const
{
    std::string monthStr;

    if (month < 10)
        monthStr = "0" + std::to_string(month);
    else
        monthStr = std::to_string(month);

    return  std::to_string(year) + monthStr;
}

std::string Date::toXMLInvoiceFileName() const
{
    std::string dayStr;
    if (day < 10)
        dayStr = "0" + std::to_string(day);
    else
        dayStr = std::to_string(day);

    return toXMLReportFileName() + dayStr;
}

bool Date::Date::operator == (const Date& other) const
{
    if (year == other.year)
    {
        if (month == other.month)
        {
            if (day == other.day)
            {
                return true;
            }
        }
    }

    return false;
}

bool Date::operator!=(const Date& other) const
{
    return !(*this == other);
}

Date Date::getMaxDateOfMonth() const { return Date{ getMaxDayOfMonth(), month, year }; }
bool Date::isTheSameMonthAs(const Date& date) const { return this->year == date.year && this->month == date.month; }

int Date::getAge(const Date& currentDate)
{
    int age = currentDate.year - year;

    if ((currentDate.month == month && currentDate.day < day) ||
        month > currentDate.month)
        age--;

    return std::max(-1, age);
}

bool Date::Date::operator < (const Date& other) const
{
    if (year < other.year) return true;
    if (year == other.year && month < other.month) return true;
    if (year == other.year && month == other.month && day < other.day) return true;

    return false;
}


bool Date::Date::operator > (const Date& other) const
{
    if (year > other.year) return true;
    if (year == other.year && month > other.month) return true;
    if (year == other.year && month == other.month && day > other.day) return true;
    return false;
}

bool Date::Date::operator >= (const Date& other) const { return *this == other || *this > other; }
bool Date::Date::operator <= (const Date& other) const { return *this == other || *this < other; }

bool Date::isWeekend() const
{
    QDate d(year, month, day);

    return d.dayOfWeek() == 6 || d.dayOfWeek() == 7;
}

int Date::getWorkdaysOfMonth(int month, int year)
{
    int workDays{0};

    QDate monthBegin(year, month, 1);
    QDate monthEnd(year, month, monthBegin.daysInMonth());

    while(monthBegin <= monthEnd)
    {
        int dayOfWeek = monthBegin.dayOfWeek();

        if (dayOfWeek != 6 && dayOfWeek != 7) workDays++;

        monthBegin = monthBegin.addDays(1);
    }

    return workDays;
}

bool Date::isInitialized() const
{
    return (year == 1900 && month == 1 && day == 1);
}

Date Date::currentDate() { return Date(currentDay(), currentMonth(), currentYear()); }

int Date::currentDay() { return QDate::currentDate().day(); }
int Date::currentMonth() { return QDate::currentDate().month(); }
int Date::currentYear() { return QDate::currentDate().year(); }

Date Date::getDateFromXmlFormat(const std::string& yyyy_dash_MM_dash_dd)
{
    return Date{
           std::stoi(yyyy_dash_MM_dash_dd.substr(8, 2)),
           std::stoi(yyyy_dash_MM_dash_dd.substr(5, 2)),
           std::stoi(yyyy_dash_MM_dash_dd.substr(0, 4))
    };
}
