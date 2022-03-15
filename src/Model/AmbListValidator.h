#pragma once

#include <string>

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
	Date ambListDate;
	std::vector<Procedure> m_procedures; //only NZOK procedures
	
	bool noDuplicates();
	bool validateTypeToStatus(const Tooth& tooth, const Procedure& p);
	bool validatePermaTemp(const Tooth& tooth, const Procedure& p);
	bool isValidAccordingToDb();
	bool dateIsValid();
	bool examIsFirst();

public:
	AmbListValidator(const AmbList& list, const Patient& patient);
	bool ambListIsValid();
	const std::string& getErrorMsg();

};

