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

#include "Model/CityCode.h"
#include "Network/ReplyHandler.h"


class PatientDialogPresenter : private ReplyHandler
{
	std::optional<Patient> _patient;

	IPatientDialog* view;

	EgnValidator egn_validator;
	Ln4Validator ln4_validator;
	DateValidator birth_validator;
	NameValidator name_validator;
	HIRBNoValidator hirb_validator;
	CityValidator city_validator;
	CyrillicValidator cyrillic_validator;

	//data not present in view:
	long long rowid{ 0 };
	std::string allergies;
	std::string pastDiseases;
	std::string currentDiseases;


	bool inputIsValid(AbstractUIElement* uiElement);
	Patient getPatientFromView();
	void setPatientToView(const Patient& patient);

	void getReply(const std::string& reply);

public:
	PatientDialogPresenter();
	PatientDialogPresenter(const Patient& patient);

	void cityChanged();
	std::optional<Patient> open();

	void changePatientType(int index);
	void activeHirbnoCheck();
	void activeHirbnoReplyCallback(const std::string& reply);
	void searchDbForPatient(int type);
	void accept();

	void setView(IPatientDialog* view);

};

