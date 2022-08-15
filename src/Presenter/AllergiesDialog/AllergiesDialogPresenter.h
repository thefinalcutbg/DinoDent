#pragma once
#include "View/AllergiesDialog/IAllergiesDialog.h"
#include "Model/Patient.h"
#include <optional>

struct Allergies
{
	std::string allergies;
	std::string current;
	std::string past;
};


class AllergiesDialogPresenter
{
	IAllergiesDialog* view;
	std::optional<Allergies> allergies;

public:
	AllergiesDialogPresenter(const Patient& patient);
	void setView(IAllergiesDialog* view);
	void okClicked();
	std::optional<Allergies> openDialog();
};

