#pragma once

#include <string>
#include "Database/DbProcedure.h"
#include "Model/AmbList.h"
#include "Model/Patient.h"
//class AmbList;
//class Patient;
//class Tooth;
//class Procedure;

class AmbListValidator
{
	std::string _error;
	
	const AmbList& ambList;
	const Patient& patient;
	std::vector<Procedure> m_procedures; //only NZOK procedures

	DbProcedure _db;
	
	bool noDuplicates();
	bool validateTypeToStatus(const Tooth& tooth, const Procedure& p);
	bool validatePermaTemp(const Tooth& tooth, const Procedure& p);
	bool isValidAccordingToDb();
	bool dateIsValid(const Procedure& p);
	bool examIsFirst();

public:
	AmbListValidator(const AmbList& list, const Patient& patient);
	bool ambListIsValid();
	const std::string& getErrorMsg();

};

