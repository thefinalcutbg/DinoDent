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
