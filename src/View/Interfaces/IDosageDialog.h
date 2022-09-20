#pragma once
#include <string>
#include <vector>

struct Dosage;

class IDosageDialog
{
public:
	virtual void setBoundsList(const std::vector<std::string>& list) = 0;
	virtual void setPeriodList(const std::vector<std::string>& list) = 0;
	virtual void setRouteList(const std::vector<std::string>& list) = 0;

	virtual void setWhenFormCompletionList(const std::vector<std::string>& list) = 0;
	virtual void setDoseFormCompletionList(const std::vector<std::string>& list) = 0;

	virtual void setWhenTags(const std::vector<std::string>& tags, bool offsetAllowed) = 0;

	virtual void setDosageUnit(const std::string& unitName) = 0;

	virtual void setRouteString(const std::string& route) = 0;

	virtual void setParsed(const std::string& parsed) = 0;

	virtual void setDosage(const Dosage& d) = 0;

	virtual bool fieldsAreValid() = 0;
	virtual void closeUi() = 0;


};