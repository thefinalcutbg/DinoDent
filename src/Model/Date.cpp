#include "Date.h"

#include <set>
#include <QDate>
#include <JsonCpp/json.h>
#include "Resources.h"

int Date::monthDays[12]{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

constexpr const char* monthNames[12]{
    "Януари", "Февруари", "Март", "Април", "Май", "Юни",
    "Юли", "Август", "Септември", "Октомври", "Ноември", "Декември"
};

std::set<Date> s_holidays;
std::set<Date> s_workdays;

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

Date::Date(const std::string& str)
{
    if (str.empty()) return;

    if (str[4] == '-') {
        day = (stoi(str.substr(8, 2)));
        month = (stoi(str.substr(5, 2)));
        year = (stoi(str.substr(0, 4)));
        return;
    }

}


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
    return getMaxDayOfMonth(month, year);
}

int Date::getMaxDayOfMonth(int month, int year)
{
    if (month < 1 || month > 12) return 0;

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

Date Date::tomorrow() const
{
    if (day != getMaxDayOfMonth()) return Date{ day + 1, month, year };

    if (month != 12) return Date{ 1, month + 1, year };

    return Date{ 1, 1, year + 1 };
}

bool Date::isDefault() const
{
    return (day == 1 && month == 1 && year == 1900);
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


std::string Date::toBgStandard(bool suffix) const
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
        yearStr += " г.";

    return dayStr + "." + monthStr + "." + yearStr;
}

std::string Date::to8601() const
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

std::string Date::getMonthName() const
{
    return monthNames[month - 1];
}

bool Date::isFromPreviousMonths(const Date& other) const
{
    if (year < other.year) return true;
    if (year == other.year && month < other.month) return true;
    return false;
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
    if (s_workdays.count(*this)) return false;

    if (s_holidays.count(*this)) return true;

    QDate d(year, month, day);

    return d.dayOfWeek() == 6 || d.dayOfWeek() == 7;
}

void Date::initializeHolidays()
{
    s_holidays.clear();
    s_workdays.clear();

    Json::Value holidaysJson;
    
    Json::Reader().parse(Resources::fromPath(":/json/json_holidays.json"), holidaysJson);

    for (auto& val : holidaysJson["holidays"])
    {
        s_holidays.insert(val.asString());
    }

    for (auto& val : holidaysJson["workdays"])
    {
        s_workdays.insert(val.asString());
    }
}

int Date::getWorkdaysOfMonth(int month, int year)
{
    int workDays{0};
    
    int dayFirst = 1;
    int dayLast = getMaxDayOfMonth(month, year);

    for (int i = 1; i <= dayLast; i++)
    {
        if (!Date(i, month, year).isWeekend()) workDays++;
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

