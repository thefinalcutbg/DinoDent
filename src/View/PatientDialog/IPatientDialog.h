#pragma once

class Patient;
class Validator;

enum PatientField { id, birthdate, fname, mname, lname, city, address, hirbno, phone };

#include "View/uiComponents/AbstractLineEdit.h"

class IPatientDialog
{

public:

	virtual void close() = 0;
	virtual void setLn4View(bool show) = 0;
	virtual void setEditMode(bool editMode) = 0;
	virtual void setCodeInfo(const std::string& codeInfo) = 0;
	virtual void setPatient(const Patient& patient) = 0;
	virtual void resetFields() = 0;
	virtual Patient getPatient() = 0;

	virtual AbstractLineEdit* lineEdit(PatientField field) = 0;
};

