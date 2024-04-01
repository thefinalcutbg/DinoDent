#pragma once

class Patient;
class Validator;
enum class Insured;

class IPatientDialog
{

public:

	virtual void setTitle(const std::string& title) = 0;
	virtual void close() = 0;
	virtual void setHirbno(const std::string& hirbno) = 0;
	virtual void setEditMode(bool editMode) = 0;
	virtual void setPatient(const Patient& patient) = 0;
	virtual Patient getPatient() = 0;
	virtual bool inputFieldsAreValid() = 0;

};

