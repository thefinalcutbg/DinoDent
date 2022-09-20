#include "Time.h"
#include "Date.h"
#include "FreeFunctions.h"
#include "qdatetime.h"

Time::Time(const std::string& ISO8601)
{
	if (ISO8601.size() < 19) return;

	hour = std::stoi(ISO8601.substr(11, 2));
	minutes = std::stoi(ISO8601.substr(14, 2));
	sec = std::stoi(ISO8601.substr(18, 2));
}

std::string Time::to8601(const Date& date, char separator) const
{
	std::string result;
	result.reserve(19);

	result += date.to8601();
	result += separator;
	result +=
		FreeFn::leadZeroes(hour, 2) + ":" +
		FreeFn::leadZeroes(minutes, 2) + ":" +
		FreeFn::leadZeroes(sec, 2)
		;

	return result;
}

Time Time::currentTime()
{
	return Time(QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate).toStdString());
}

