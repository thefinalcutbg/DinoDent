#pragma once


#include <optional>
#include <array>


#include "Model/Validators/DateValidator.h"
#include "Model/Validators/NameValidator.h"
#include "Model/Validators/PatientValidators.h"
#include "Model/Patient.h"
#include "Model/Date.h"

#include "View/Interfaces/IPatientDialog.h"
#include "View/Interfaces/AbstractLineEdit.h"
#include "View/Interfaces/AbstractComboBox.h"
#include "View/Interfaces/AbstractLabel.h"


#include "Network/PIS/GetHirbnoService.h"


class PatientDialogPresenter
{
	std::optional<Patient> m_patient;

	IPatientDialog* view;

	EgnValidator egn_validator;
	Ln4Validator ln4_validator;
	DateValidator birth_validator;
	NameValidator name_validator;
	HIRBNoValidator hirb_validator;
	CityValidator city_validator;
	CyrillicValidator cyrillic_validator;

	GetHirbnoService hirbnoHandler;

	//data not present in view:
	long long rowid{ 0 };
	MedicalStatuses medStats;

	Patient getPatientFromView();
	void setPatientToView(const Patient& patient);
	bool viewIsValid();

public:
	PatientDialogPresenter();
	PatientDialogPresenter(const Patient& patient);
	std::optional<Patient> open();

	void changePatientType(int index);
	void setHirbno(const std::string& hirbno);
	void checkHirbno();
	void searchDbForPatient(int type);
	void accept();

	void setView(IPatientDialog* view);

};

