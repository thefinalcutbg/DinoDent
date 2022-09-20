#pragma once
#include <string>

namespace DbNotes
{
	std::string getNote(long long patientRowId, int toothIdx);
	void saveNote(const std::string& note, long long patientRowId, int toothIdx);
};

