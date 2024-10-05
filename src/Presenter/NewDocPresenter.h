#pragma once

#include <optional>
#include "Model/Patient.h"

class NewDocDialog;

class NewDocPresenter
{
	std::optional<Patient> result;

	std::string title;

	NewDocDialog* view{ nullptr };

public:

	NewDocPresenter(const std::string& title);

	void newPatient();
	void patientSelected(long long rowid);

	std::optional<Patient> open();

};