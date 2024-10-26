#pragma once
#include <string>
#include <QDateTime>

struct Patient;

struct Calendar
{
	std::string summary;
	std::string id;
	std::string etag;
};

struct CalendarEvent
{
	CalendarEvent(const Patient& p);
	CalendarEvent() {}
	std::string id;
	std::string summary;
	std::string description;
	QDateTime start;
	QDateTime end;

	std::string json; //all other parameters not supported by the application
	
	//app specific properties:
	std::string patientBirth;
	std::string patientFname;
};
