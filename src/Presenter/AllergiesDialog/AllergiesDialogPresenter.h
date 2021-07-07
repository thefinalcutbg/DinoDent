#pragma once
#include "View/AllergiesDialog/IAllergiesDialog.h"
#include "Database/DbPatient.h"
#include <optional>

struct Allergies
{
	Allergies(const std::string &allergies, const std::string& current, const std::string& past) :
		allergies(allergies), current(current), past(past) {};
	std::string allergies;
	std::string current;
	std::string past;
};


class AllergiesDialogPresenter
{
	IAllergiesDialog* view;
	DbPatient amb_db;
	const Patient* patient;
	std::optional<Allergies> allergies;

public:
	AllergiesDialogPresenter(const Patient& patient);
	void setView(IAllergiesDialog* view);
	void okClicked();
	std::optional<Allergies> openDialog();
};

