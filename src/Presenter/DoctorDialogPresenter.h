#pragma once
#include "Model/UserStructs.h"
#include "Model/Validators/NameValidator.h"
#include "Model/Validators/CommonValidators.h"
#include "Model/Validators/PatientValidators.h"
#include "Model/Validators/LpkValidator.h"
#include "View/Interfaces/IDoctorSettingsDialog.h"

class DoctorDialogPresenter
{
	IDoctorSettingsDialog* view{nullptr};

	std::string current_LPK;

	std::optional<Doctor> result;

	NhifSpecialty nhif_specialty{ NhifSpecialty::None };

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

