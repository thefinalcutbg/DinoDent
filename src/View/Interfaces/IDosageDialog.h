#pragma once
#include <string>
#include <vector>


class IDosageDialog
{
public:
	virtual void setDurationList(const std::vector<std::string>& list) = 0;
	virtual void setPeriodList(const std::vector<std::string>& list) = 0;
	virtual void setRouteList(const std::vector<std::string>& list) = 0;
	virtual void setDoseFormCompletionList(const std::vector<std::string>& list) = 0;
	virtual void setDosageUnit(const std::string& unitName) = 0;
	virtual void setDosageLabel(const std::string& label) = 0;
	virtual void setRouteString(const std::string& route) = 0;

};