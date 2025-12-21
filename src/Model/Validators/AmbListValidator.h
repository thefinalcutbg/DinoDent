#pragma once

#include <string>

#include "Model/Dental/AmbList.h"
#include "Model/Patient.h"
#include "Model/TableStructs.h"

struct AmbListValidator
{
	std::string _error;
	
	const AmbList& ambList;
	const Patient& patient;
	Date ambListDate;
	std::vector<Procedure> m_procedures; //only NHIF procedures

    bool validatePackage(const std::vector<ProcedureSummary>& source, const std::vector<Procedure>& procedures, const std::string& sourceName);

	bool noDuplicates();
	bool validateTypeToStatus(const Tooth& tooth, const Procedure& p);
	bool validatePermaTemp(const Tooth& tooth, const Procedure& p);
	bool isValidAccordingToDb();
    bool isValidAccordingToHIS();
	bool dateIsValid();
	bool examIsFirst();
	bool isNhifInWeekend();
	bool procedureOnExtractedTooth();
	std::optional<std::pair<Date, int>> exceededDailyLimit();

public:
	AmbListValidator(const AmbList& list, const Patient& patient);
	bool ambListIsValid();
	const std::string& getErrorMsg();
};

