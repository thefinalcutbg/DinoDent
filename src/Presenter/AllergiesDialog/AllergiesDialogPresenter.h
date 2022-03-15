#pragma once
#include "View/AllergiesDialog/IAllergiesDialog.h"
#include "Model/Patient.h"
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
	const Patient* patient;
	std::optional<Allergies> allergies;

public:
	AllergiesDialogPresenter(const Patient& patient);
	void setView(IAllergiesDialog* view);
	void okClicked();
	std::optional<Allergies> openDialog();
};

