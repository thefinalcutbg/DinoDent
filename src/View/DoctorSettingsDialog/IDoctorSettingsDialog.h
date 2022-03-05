#pragma once
#include "Model/User/User.h"


class DoctorDialogPresenter;
class AbstractLineEdit;


namespace DoctorFields
{
	enum Field { EGN, FirstName, MiddleName, LastName, Password, LPK };
}


class IDoctorSettingsDialog
{
public:
	virtual void setDoctor(const Doctor& doctor) = 0;
	virtual Doctor getDoctor() = 0;
	virtual AbstractLineEdit* lineEdit(DoctorFields::Field field) = 0;
	virtual void close() = 0;
	virtual void setToReadOnly() = 0;
};