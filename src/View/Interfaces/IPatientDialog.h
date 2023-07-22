#pragma once

class Patient;
class Validator;
enum class Insured;
enum PatientField { id, fname, mname, lname, city, address, hirbno, phone };

#include "View/Interfaces/AbstractLineEdit.h"
#include "View/Interfaces/AbstractDateEdit.h"
class IPatientDialog
{

public:

	virtual void setTitle(const std::string& title) = 0;

	virtual void close() = 0;
	virtual void setHirbno(const std::string& hirbno) = 0;
	virtual void setType(Patient::Type type) = 0;
	virtual void setEditMode(bool editMode) = 0;
	virtual void setPatient(const Patient& patient) = 0;
	virtual void resetFields() = 0;
	virtual Patient getPatient() = 0;

	virtual AbstractLineEdit* lineEdit(PatientField field) = 0;
	virtual AbstractDateEdit* dateEdit() = 0;
};

