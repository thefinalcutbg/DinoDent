#pragma once
#include <vector>

#include "View/Interfaces/AbstractLineEdit.h"
#include "View/Interfaces/AbstractDateEdit.h"
#include "Model/UserStructs.h"

constexpr int practiceTextFieldCount = 11;

class PracticeSettingsPresenter;
struct Practice;

namespace PracticeTextFields
{
	enum Field{Name, RZI, Bulstat, FirmAddress, ActivityAddress, Password, VAT, NZOKContract, NZOKShortName, NraPass, SelfInsuredId  };
};

class IPracticeSettings
{
public:
	virtual void setPractice(const Practice& practice) = 0;
	virtual Practice getPractice() = 0;
	virtual AbstractLineEdit* lineEdit(PracticeTextFields::Field field) = 0;
	virtual AbstractDateEdit* dateEdit() = 0;
	virtual int legalForm() = 0;
	virtual void setPresenter(PracticeSettingsPresenter* presenter) = 0;
	virtual void hidePassword() = 0;
	virtual void setDoctorList(const std::vector<PracticeDoctor>& doctors) = 0;
	virtual void setDoctorProperties(bool admin, NhifSpecialty specialty) = 0;
	virtual void replaceCurrentItem(const PracticeDoctor& item) = 0;
};