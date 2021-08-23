#pragma once

#include "AbstractORM.h"

class DbNotes : public AbstractORM
{
public:
	std::string getNote(const std::string& patientID, int toothIdx);
	void saveNote(const std::string& note, const std::string& patientID, int toothIdx);
};

