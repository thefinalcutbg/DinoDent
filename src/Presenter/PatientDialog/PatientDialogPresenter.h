#pragma once


#include <optional>
#include <array>


#include "Model/Validator/DateValidator.h"
#include "Model/Validator/NameValidator.h"
#include "Model/Validator/PatientValidators.h"
#include "Model/Patient.h"
#include "Model/Date.h"

#include "View/PatientDialog/IPatientDialog.h"

#include "View/uiComponents/AbstractLineEdit.h"
#include "View/uiComponents/AbstractComboBox.h"
#include "View/uiComponents/AbstractLabel.h"


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
	std::string allergies;
	std::string pastDiseases;
	std::string currentDiseases;

	bool inputIsValid(AbstractUIElement* uiElement);
	Patient getPatientFromView();
	void setPatientToView(const Patient& patient);

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

