#pragma once
#include <string>
#include <ctime>

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

	static Date GetDateFromEgn(const std::string& egn);
	static std::string toString(const Date& date);
	static Date getCurrentDate();
	static int currentDay();
	static int currentMonth();
	static int currentYear();
	static bool isLeapYear(int year);
	static int getMaxDayOfMonth(int month, int year);


	bool Date::operator < (const Date& other) const;
	bool Date::operator > (const Date& other) const;
	bool Date::operator == (const Date& other) const;

};