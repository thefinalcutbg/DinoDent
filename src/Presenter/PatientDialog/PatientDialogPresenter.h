#pragma once

#include "Model/Patient.h"
#include "Model/DbPatient.h"
#include "Model/Date.h"

#include "Database/Database.h"
#include "View/PatientDialog/IPatientDialog.h"

#include "View/uiComponents/AbstractLineEdit.h"
#include "View/uiComponents/AbstractComboBox.h"
#include "View/uiComponents/AbstractLabel.h"
#include "uiObserver.h"
#include "Model/CityCode.h"
#include <array>

enum patientFields{id, birthdate, fname, mname, lname, city, address, hirbno, phone};

class PatientDialogPresenter : public uiObserver
{
	bool new_patient;
	bool egn_form;
	bool edited;

	IPatientDialog* view;
	DbPatient database;
	PatientDialogRequestor* requestor;

	CityCode city_code;

	EgnValidator egn_validator;
	Ln4Validator ln4_validator;
	BirthValidator birth_validator;
	NameValidator name_validator;
	HIRBNoValidator hirb_validator;
	CityValidator city_validator;
	NameReformator name_reformator;

	std::string allergies;
	std::string pastDiseases;
	std::string currentDiseases;

	std::array<AbstractLineEdit*, 9> patient_field;

	AbstractComboBox* patientTypeCombo;
	AbstractComboBox* sexCombo;
	AbstractLabel* cityCodeLabel;

	Patient getPatientFromView();
	void setPatientToView(const Patient& patient);
	void searchDbForPatient();
	void setCityCodesLabel();
	void resetForm();


public:
	PatientDialogPresenter(IPatientDialog *IPatientForm, Database* database);

	void setPatientFields(std::array<AbstractLineEdit*, 9>, AbstractComboBox* patientType, AbstractComboBox* sexCombo, AbstractLabel* cityCodeLabel);

	void open(PatientDialogRequestor* requestor);
	void open(PatientDialogRequestor* requestor, Patient patient);

	void EgnTypeDialog();
	void Ln4TypeDialog();

	void accept();
	void handleNotifications(Notification notification);

};

