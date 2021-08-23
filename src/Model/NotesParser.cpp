#include "NotesParser.h"

std::string NotesParser::write(const std::array<std::string, 32>& notes)
{
	std::string notesString;

	Json::Value jNotes;

	for (int i = 0; i < 32; i++)
	{
		const auto& note = notes[i];

		if (!note.empty())
		{
			if (!jNotes.isMember("Notes"))
				jNotes["Notes"] = Json::Value(Json::arrayValue);

			Json::Value parameters;
			parameters["idx"] = i;
			parameters["text"] = note;
			jNotes["Notes"].append(parameters);
		}

	}

	return notesString;
}

void NotesParser::parse(const std::string& jsonString, std::array<std::string, 32>& notes)
{
	Json::Value json;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonString, json);

	if (!parsingSuccessful) {
		return;
	}

	const Json::Value& jNotes = json["Notes"];

	for (int i = 0; i < jNotes.size(); i++)
	{
		auto& note = notes[jNotes[i]["idx"].asInt()];
		note = jNotes[i]["text"].asString();
	}

}
