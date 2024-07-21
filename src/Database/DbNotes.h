#pragma once
#include <string>

namespace DbNotes
{
	std::string getNote(long long patientRowId, int toothIdx);
	bool saveNote(const std::string& note, long long patientRowId, int toothIdx);
};

