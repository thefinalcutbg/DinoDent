#pragma once
#include "Model/User/UserStructs.h"


class DoctorDialogPresenter;
class AbstractLineEdit;


namespace DoctorFields
{
	enum Field { LPK, EGN, FirstName, MiddleName, LastName, Phone, Password };
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