#pragma once
#include "View/AllergiesDialog/IAllergiesDialog.h"
#include "Model/DbPatient.h"

struct Allergies
{
	Allergies(std::string allergies, std::string current, std::string past) :
		allergies(allergies), current(current), past(past) {};
	std::string allergies;
	std::string current;
	std::string past;
};

class AllergiesDialogRequestor
{
public:
	virtual void setAllergies(Allergies allergies) = 0;
};

class AllergiesDialogPresenter
{
	IAllergiesDialog* view;
	DbPatient database;
	std::string patient_id;
	AllergiesDialogRequestor* requestor;
public:
	AllergiesDialogPresenter(IAllergiesDialog* view, Database* database);
	void setAllergies(std::string allergies, std::string current, std::string past);
	void openDialog(AllergiesDialogRequestor* requestor, Patient patient);
};

