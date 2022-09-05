#pragma once
#include <string>
class MedicationPresenter;

class IMedicationDialog
{
public:
	virtual void setQuantityListNames(const std::string& pack, const std::string& form) = 0;
	virtual void setFormLabel(const std::string& formName) = 0;
};