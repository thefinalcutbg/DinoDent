#pragma once


#include <optional>
#include <array>

#include "Database/DbPatient.h"
#include "Model/Validator/DateValidator.h"
#include "Model/Validator/NameValidator.h"
#include "Model/Patient.h"
#include "Model/Date.h"

#include "Database/Database.h"
#include "View/PatientDialog/IPatientDialog.h"

#include "View/uiComponents/AbstractLineEdit.h"
#include "View/uiComponents/AbstractComboBox.h"
#include "View/uiComponents/AbstractLabel.h"

#include "Model/CityCode.h"



class PatientDialogPresenter
{
	bool new_patient;

	std::optional<Patient> _patient;

	IPatientDialog* view;
	DbPatient database;

	CityCode city_code;

	EgnValidator egn_validator;
	Ln4Validator ln4_validator;
	DateValidator birth_validator;
	NameValidator name_validator;
	HIRBNoValidator hirb_validator;
	CityValidator city_validator;
	CyrillicValidator cyrillic_validator;

	std::string allergies;
	std::string pastDiseases;
	std::string currentDiseases;


	bool inputIsValid(AbstractUIElement* uiElement);
	Patient getPatientFromView();
	void setPatientToView(const Patient& patient);

public:
	PatientDialogPresenter();
	PatientDialogPresenter(const Patient& patient);

	void cityChanged();
	std::optional<Patient> open();

	void changePatientType(int index);

	void searchDbForPatient(int type);
	void accept();

	void setView(IPatientDialog* view);

};

