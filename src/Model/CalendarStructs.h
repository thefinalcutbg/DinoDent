#pragma once
#include <string>
#include <QDateTime>

struct Calendar
{
	std::string summary;
	std::string id;
	std::string etag;
};

struct CalendarEvent
{
	
	std::string id;
	std::string summary;
	QDateTime start;
	QDateTime end;
	std::string json; //all other parameters not supported by the application

};


struct CalendarCacheKey {
	int calendarIdx;
	int year;
	int weekNumber;

	bool operator==(const CalendarCacheKey& other) const
	{
		return (calendarIdx == other.calendarIdx
			&& year == other.year
			&& weekNumber == other.weekNumber);
	}
};

template <>
struct std::hash<CalendarCacheKey>
{
	std::size_t operator() (const CalendarCacheKey& node) const
	{
		std::size_t h1 = std::hash<int>()(node.calendarIdx);
		std::size_t h2 = std::hash<int>()(node.year);
		std::size_t h3 = std::hash<int>()(node.weekNumber);
		return h1 ^ h2 ^ h3;
	}
};
