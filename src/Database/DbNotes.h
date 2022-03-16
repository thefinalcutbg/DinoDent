#pragma once
#include <string>

namespace DbNotes
{
	std::string getNote(const std::string& patientID, int toothIdx);
	void saveNote(const std::string& note, const std::string& patientID, int toothIdx);
};

