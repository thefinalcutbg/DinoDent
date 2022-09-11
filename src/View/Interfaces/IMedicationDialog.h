#pragma once
#include <string>
#include <vector>

class MedicationPresenter;
struct Medication;
class IMedicationDialog
{
public:
	virtual void setQuantityListNames(const std::string& pack, const std::string& form) = 0;
	virtual void setFormLabel(const std::string& formName) = 0;
	virtual void setDosageList(const std::vector<std::string> dosageList) = 0;
	virtual bool fieldsAreValid() = 0;
	virtual void setMedication(const Medication& m) = 0;
	virtual void closeUi() = 0;
};