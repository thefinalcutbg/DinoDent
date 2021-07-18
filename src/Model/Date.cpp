#include "Date.h"


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
    month(stoi(dd_dot_MM_dot_yyyy.substr(3, 4))),
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

Date Date::GetDateFromEgn(const std::string& egn)
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

std::string Date::toString(const Date& date)
{
    std::string day;

    if (date.day < 10)
        day = "0" + std::to_string(date.day);
    else
        day = std::to_string(date.day);

    std::string month;

    if (date.month < 10)
        month = "0" + std::to_string(date.month);
    else
        month = std::to_string(date.month);

    return day + "." + month + "." + std::to_string(date.year);
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

Date Date::getMaxDateOfMonth() const { return Date{ getMaxDayOfMonth(), month, year }; }
bool Date::isTheSameMonthAs(const Date& date) const { return this->year == date.year && this->month == date.month; }

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

Date Date::currentDate() { return Date(currentDay(), currentMonth(), currentYear()); }

#include <QDate>
int Date::currentDay() { return QDate::currentDate().day(); }
int Date::currentMonth() { return QDate::currentDate().month(); }
int Date::currentYear() { return QDate::currentDate().year(); }
