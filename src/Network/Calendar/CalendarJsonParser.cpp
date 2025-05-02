#include "CalendarJsonParser.h"
#include <json/json.h>
#include <QDateTime>

void parseEventSpecificProperties(Json::Value& json, CalendarEvent& event)
{

	//parsing app specific
	if (
		json.isMember("extendedProperties") &&
		json["extendedProperties"].isMember("shared") &&
		json["extendedProperties"]["shared"].isMember("app") &&
		json["extendedProperties"]["shared"]["app"].asString() == "DinoDent"
		)
	{
		event.patientBirth = json["extendedProperties"]["shared"]["birth"].asString();
		event.patientFname = json["extendedProperties"]["shared"]["fname"].asString();
		json.removeMember("extendedProperties");
	}
}

std::vector<Calendar> CalendarJsonParser::parseCalendarList(const std::string& response)
{
	std::vector<Calendar> result;

	Json::Reader r;
	Json::Value jsonObject;
	r.parse(response, jsonObject);

	result.reserve(jsonObject["items"].size());

	for (auto& item : jsonObject["items"]) {

		result.emplace_back(Calendar{
			.summary = item["summary"].asString(),
			.id = item["id"].asString(),
            .etag = item["etag"].asString(),
            .timeZone = item["timeZone"].asString()
		});
	}

	return result;
}

CalendarEvent CalendarJsonParser::parseEvent(const std::string& response)
{
	Json::Reader r;
	Json::Value json;
	r.parse(response, json);

	if (json["kind"].asString() != "calendar#event") return CalendarEvent{};

	CalendarEvent e;
	e.id = json["id"].asString();
	e.summary = json["summary"].asString(),
	e.description = json["description"].asString();
	e.start = QDateTime::fromString(json["start"]["dateTime"].asString().c_str(), Qt::DateFormat::ISODate).toLocalTime();
	e.end = QDateTime::fromString(json["end"]["dateTime"].asString().c_str(), Qt::DateFormat::ISODate).toLocalTime();
	
	json.removeMember("id");
	json.removeMember("summary");
	json.removeMember("start");
	json.removeMember("end");
	json.removeMember("kind");
	json.removeMember("description");

	parseEventSpecificProperties(json, e);

	e.json = Json::FastWriter().write(json);

	return e;
	
}

std::vector<CalendarEvent> CalendarJsonParser::parseEventList(const std::string& response)
{
	std::vector<CalendarEvent> result;
	
	Json::Reader r;
	Json::Value json;
	r.parse(response, json);

	result.reserve(json["items"].size());

	if (json["kind"].asString() != "calendar#events") return {};

	for (auto& item : json["items"]) {

		CalendarEvent e;

		e.id = item["id"].asString();
		e.summary = item["summary"].asString();
		e.description = item["description"].asString();
		e.start = QDateTime::fromString(item["start"]["dateTime"].asString().c_str(), Qt::DateFormat::ISODate).toLocalTime();
		e.end = QDateTime::fromString(item["end"]["dateTime"].asString().c_str(), Qt::DateFormat::ISODate).toLocalTime();
		
		item.removeMember("id");
		item.removeMember("summary");
		item.removeMember("start");
		item.removeMember("end");
		item.removeMember("description");

		parseEventSpecificProperties(item, e);

		e.json = Json::FastWriter().write(item);

		result.push_back(e);
	};

	return result;
}



std::string CalendarJsonParser::writeEventQuery(const CalendarEvent& event, const std::string& timeZone)
{
	Json::Value json;

	Json::Reader r;

	r.parse(event.json, json);

	Json::Value start;

	start["dateTime"] = event.start.toString(Qt::DateFormat::ISODate).toStdString();
    start["timeZone"] = "Europe/Sofia";

	json["start"] = start;

	Json::Value end;

	end["dateTime"] = event.end.toString(Qt::DateFormat::ISODate).toStdString();
    end["timeZone"] = "Europe/Sofia";

	json["end"] = end;

	json["summary"] = event.summary;

	json["description"] = event.description;

	if (event.patientFname.size()) {
		//writing the specific properties
		Json::Value prop;
		prop["app"] = "DinoDent";
		prop["fname"] = event.patientFname;
		prop["birth"] = event.patientBirth;

		Json::Value appSpecific;
		appSpecific["shared"] = prop;

		json["extendedProperties"] = appSpecific;
	}

	return Json::FastWriter().write(json);
}
