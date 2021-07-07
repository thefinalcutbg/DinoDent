#pragma once

#include <string>

class Date
{

    static int monthDays[12];

public:
    int day;
    int month;
    int year;

    Date();
    Date(int day, int month, int year);
    Date(const std::string& dd_dot_MM_dot_yyyy);

    //all my troubles seem so far away
    Date yesterday();

    static Date GetDateFromEgn(const std::string& egn);
    static std::string toString(const Date& date);
    static Date currentDate();

    static int currentDay();
    static int currentMonth();
    static int currentYear();

    static bool isLeapYear(int year);
    int getMaxDayOfMonth();
    bool isTheSameMonthAs(const Date& date);

    bool operator < (const Date& other) const;
    bool operator > (const Date& other) const;
    bool operator == (const Date& other) const;
    bool operator >= (const Date& other) const;
    bool operator <= (const Date& other) const;
};
