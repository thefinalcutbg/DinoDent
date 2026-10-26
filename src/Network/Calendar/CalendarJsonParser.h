#pragma once

#include <vector>
#include "Model/CalendarStructs.h"
#include <map>
#include <optional>

namespace CalendarJsonParser
{
    struct SearchEvent {
        std::string summary;
        QDate date;
        QDateTime start;
        bool allDay = false;
    };

    struct SearchResult {
        std::vector<SearchEvent> events;
        std::string nextPageToken;
    };

    // Invalid JSON or an API error returns nullopt; an empty page is valid.
    std::optional<SearchResult> parseSearchResult(const std::string& response);

	std::vector<Calendar> parseCalendarList(const std::string& response);
	CalendarEvent parseEvent(const std::string& response);
	std::vector<CalendarEvent> parseEventList(const std::string& response);
    std::string writeEventQuery(const CalendarEvent& event, const std::string& timeZone);
    std::map<std::string, QColor> parseCalendarColors(const std::string& response);

}
