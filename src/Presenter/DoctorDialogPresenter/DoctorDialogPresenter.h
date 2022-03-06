#pragma once
#include "Model/User/User.h"
#include "Model/Validator/NameValidator.h"
#include "Model/Validator/CommonValidators.h"
#include "Model/Validator/PatientValidators.h"
#include "Model/Validator/LpkValidator.h"
#include "View/DoctorSettingsDialog/IDoctorSettingsDialog.h"

class DoctorDialogPresenter
{
	bool m_editMode;
	bool m_newDoctor;

	IDoctorSettingsDialog* view{nullptr};

	std::string current_LPK;

	std::optional<Doctor> result;

	EgnValidator egn_validator;
	NameValidator name_validator;
	LpkValidator lpk_validator;
	NotEmptyValidator not_emptyValidator;

public:
	//DoctorDialogPresenter();
	DoctorDialogPresenter();
	DoctorDialogPresenter(const Doctor& doctor);

	void okPressed();

	void validLPK(const std::string& validLPK);

	void setView(IDoctorSettingsDialog* view);

	std::optional<Doctor> open();
};

