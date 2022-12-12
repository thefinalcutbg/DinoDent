#pragma once

#include <string>

class Date
{

    static int monthDays[12];

public:
    int day{ 1 };
    int month{ 1 };
    int year{ 1900 };

    Date();
    Date(int day, int month, int year);
    Date(const std::string& iso8601);
    

    bool isInitialized() const;
    //all my troubles seem so far away
    Date yesterday() const;
    Date tomorrow() const;

    bool isDefault() const;

    static Date getBirthdateFromEgn(const std::string& egn);
    std::string toBgStandard(bool suffix = false) const;
    std::string to8601() const;
    std::string toXMLReportFileName() const; //required when saving the xml report file
    std::string toXMLInvoiceFileName() const;
    std::string getMonthName() const;
    bool isFromPreviousMonths(const Date& other) const;
    static Date currentDate();

    static int currentDay();
    static int currentMonth();
    static int currentYear();

    static bool isLeapYear(int year);
    int getMaxDayOfMonth() const;
    static int getMaxDayOfMonth(int month, int year);

    Date getMaxDateOfMonth() const;

    bool isTheSameMonthAs(const Date& date) const;

    int getAge(const Date& currentDate = Date::currentDate());

    bool operator < (const Date& other) const;
    bool operator > (const Date& other) const;
    bool operator == (const Date& other) const;
    bool operator != (const Date& other) const;
    bool operator >= (const Date& other) const;
    bool operator <= (const Date& other) const;

    bool isWeekend() const;
    
    static void initializeHolidays();
    static int getWorkdaysOfMonth(int month, int year);
};

