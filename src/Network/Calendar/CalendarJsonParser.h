#pragma once

#include <vector>
#include "Model/CalendarStructs.h"

namespace CalendarJsonParser
{
	std::vector<Calendar> parseCalendarList(const std::string& response);
	CalendarEvent parseEvent(const std::string& response);
	std::vector<CalendarEvent> parseEventList(const std::string& response);
	std::string writeEventQuery(const CalendarEvent& event);

}