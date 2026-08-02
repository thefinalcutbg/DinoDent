#pragma once
#include <string>
#include <QDateTime>
#include <map>
#include <QColor>

struct Patient;

struct Calendar
{
	std::string summary;
	std::string id;
	std::string etag;
    std::string timeZone;

    static inline std::map<std::string, QColor> labelIdColorMap;
};

struct CalendarEvent
{
	CalendarEvent(const Patient& p);
	CalendarEvent() {}
	std::string id;
	std::string summary;
	std::string description;
	std::string email;
	QDateTime start;
	QDateTime end;

	std::string json; //all other parameters not supported by the application
	
	//app specific properties:
	std::string patientBirth;
	std::string patientFname;

	//color from the local db
	std::string colorRgb;
    //color id from the calendar map
    std::string labelId;
};

