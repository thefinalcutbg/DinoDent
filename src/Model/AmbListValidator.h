#pragma once

#include <string>
#include "Database/DbProcedure.h"

class ListInstance;
class AmbList;
class Patient;
class Tooth;
class Procedure;

class AmbListValidator
{
	std::string _error;
	
	const AmbList& ambList;
	const Patient& patient;

	DbProcedure _db;
	
	bool noDuplicates();
	bool validateTypeToStatus(const Tooth& tooth, const Procedure& p);
	bool validatePermaTemp(const Tooth& tooth, const Procedure& p);
	bool isValidAccordingToDb();
	bool dateIsValid(const Procedure& p);

public:
	AmbListValidator(const ListInstance& list);
	bool ambListIsValid();
	const std::string& getErrorMsg();

};

