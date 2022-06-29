#pragma once

struct Practice;

#include "View/uiComponents/AbstractLineEdit.h"
#include "View/uiComponents/AbstractDateEdit.h"

constexpr int practiceTextFieldCount = 9;

class PracticeSettingsPresenter;

namespace PracticeTextFields
{
	enum Field{Name, RZI, Bulstat, Address, ActivityAddress, Password, VAT, NZOKContract, NZOKShortName, NraPass  };
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
};