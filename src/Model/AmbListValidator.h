#pragma once

#include <string>
#include "Database/DbProcedure.h"

class ListInstance;
class Tooth;
class Procedure;

class AmbListValidator
{
	std::string _error;
	
	DbProcedure _db;

	bool counter(const Procedure& p, const std::string& patientID);
	bool validateTypeToStatus(const Tooth& tooth, const Procedure& p);
	bool validatePermaTemp(const Tooth& tooth, const Procedure& p);

public:
	AmbListValidator();
	bool ambListIsValid(const ListInstance& list);
	const std::string& getErrorMsg();

};

