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

    bool isInitialized() const;
    //all my troubles seem so far away
    Date yesterday() const;

    static Date getBirthdateFromEgn(const std::string& egn);
    std::string toString() const;
    std::string toStringXML() const;
    static Date currentDate();

    static int currentDay();
    static int currentMonth();
    static int currentYear();

    static bool isLeapYear(int year);
    int getMaxDayOfMonth() const;

    Date getMaxDateOfMonth() const;
    bool isTheSameMonthAs(const Date& date) const;

    bool operator < (const Date& other) const;
    bool operator > (const Date& other) const;
    bool operator == (const Date& other) const;
    bool operator != (const Date& other) const;
    bool operator >= (const Date& other) const;
    bool operator <= (const Date& other) const;


};

