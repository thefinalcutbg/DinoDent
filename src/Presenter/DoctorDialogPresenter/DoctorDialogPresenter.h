#pragma once
#include "Model/User/User.h"
#include "Model/Validator/NameValidator.h"
#include "Model/Validator/CommonValidators.h"
#include "Model/Validator/PatientValidators.h"
#include "Model/Validator/LpkValidator.h"
#include "View/DoctorSettingsDialog/IDoctorSettingsDialog.h"

class DoctorDialogPresenter
{
	IDoctorSettingsDialog* view;

	std::string current_LPK;

	std::optional<Doctor> result;

	EgnValidator egn_validator;
	NameValidator name_validator;
	LpkValidator lpk_validator;

public:
	//DoctorDialogPresenter();
	DoctorDialogPresenter(const Doctor& doctor);

	void okPressed();

	void setView(IDoctorSettingsDialog* view);

	std::optional<Doctor> open();
};

