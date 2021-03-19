#pragma once

#include "Presenter/PatientDialog/PatientDialogPresenter.h"

class PatientDialogRequestor
{

	
public:
	virtual void setPatient(Patient patient) = 0;
};

class IPatientDialog
{

public:

	virtual void close() = 0;
	virtual void setLn4View(bool show) = 0;
	virtual void open(bool editMode) = 0;
	
};

