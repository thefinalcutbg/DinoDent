#include "PatientDialogPresenter.h"


PatientDialogPresenter::PatientDialogPresenter(IPatientDialog* patientDialog) :
	view(patientDialog),
	requestor(NULL),
	patient_field {NULL},
	new_patient(true),
	egn_form(true),
	edited(false)
{}



void PatientDialogPresenter::open(PatientDialogRequestor* requestor)
{
	this->requestor = requestor;

	resetForm();

	view->open(false);
}

void PatientDialogPresenter::open(PatientDialogRequestor* requestor, Patient patient)
{
	this->requestor = requestor;
	resetForm();
	
	setPatientToView(patient);
	new_patient = false;
	view->open(true);
}

void PatientDialogPresenter::EgnTypeDialog()
{
	view->setLn4View(false);
	egn_form = true;
	patient_field[id]->set_Validator(&egn_validator);
	patient_field[birthdate]->set_Validator(NULL);
	patient_field[mname]->set_Validator(&name_validator);

	patient_field[id]->forceValidate();

	for (int i = 0; i < patient_field.size(); i++)
		patient_field[i]->reset();
	sexCombo->setIndex(0);

}

void PatientDialogPresenter::Ln4TypeDialog()
{
	view->setLn4View(true);
	egn_form = false;
	patient_field[id]->set_Validator(&ln4_validator);
	patient_field[birthdate]->set_Validator(&birth_validator);
	patient_field[mname]->set_Validator(NULL);
	patient_field[id]->forceValidate();

	for (int i = 0; i < patient_field.size(); i++)
		patient_field[i]->reset();
	sexCombo->setIndex(0);
	
}


void PatientDialogPresenter::accept()
{
	for(int i = 0; i < patient_field.size(); i++)
	{	
		patient_field[i]->forceValidate();
		if (!patient_field[i]->isValid())
		{
			patient_field[i]->setFocusAndSelectAll();

			return;
		}
	}

	Patient patient = getPatientFromView();

	if(new_patient) database.insert(patient);
	else if (edited) database.update(patient);

	requestor->setPatient(patient);
	
	view->close();
}

void PatientDialogPresenter::handleNotifications(Notification notification)
{
	switch(notification)
	{ 
		case Notification::edited:

			edited = true;
			break;

		case Notification::validID:

			searchDbForPatient();
			break;

		case Notification::cityInfoChanged:

			setCityCodesLabel();
			break;

	}
	
}



void PatientDialogPresenter::searchDbForPatient()
{
	
	std::string lineEditID = patient_field[id]->getText();

	Patient patient = database.getPatient(lineEditID);

	allergies = patient.allergies;
	pastDiseases = patient.pastDiseases;
	currentDiseases = patient.currentDiseases;

	if (patient.id == "")
	{
		new_patient = true;
		patient.id = lineEditID;

		if (egn_form)
		{
			patient.type = 1;
			patient.birth = Date::toString(Date::GetDateFromEgn(lineEditID));
			patient.sex = Patient::getSexFromEgn(lineEditID);
		}
		else patient.type = 2;
	}
	else
		new_patient = false;

	setPatientToView(patient);
	
}

void PatientDialogPresenter::setCityCodesLabel()
{
	if (cityCodeLabel == NULL) return;

	cityCodeLabel->setLabelText(city_code.getLabel(patient_field[city]->getText()));
}

void PatientDialogPresenter::resetForm()
{
	EgnTypeDialog();

	for (int i = 0; i < patient_field.size(); i++)
		patient_field[i]->reset();

	sexCombo->setIndex(0);
	patientTypeCombo->setIndex(0);
	patientTypeCombo->disable(0);
	patient_field[id]->disable(0);
	patient_field[id]->setFocusAndSelectAll();
	patient_field[id]->setAppearence(true);
	cityCodeLabel->setLabelText("");
	new_patient = true;
	edited = false;
}

void PatientDialogPresenter::setPatientFields(std::array<AbstractLineEdit*, 9> patientFields, 
												AbstractComboBox* patientType, 
												AbstractComboBox* sexCombo,
												AbstractLabel* cityCodeLabel)
{
	this->patient_field = patientFields;

	patient_field[id]->set_Validator(&egn_validator);
	patient_field[birthdate]->set_Validator(&birth_validator);
	patient_field[fname]->set_Validator(&name_validator);
	patient_field[mname]->set_Validator(&name_validator);
	patient_field[lname]->set_Validator(&name_validator);
	patient_field[hirbno]->set_Validator(&hirb_validator);
	patient_field[city]->set_Validator(&city_validator);
	patient_field[fname]->setReformator(&name_reformator);
	patient_field[mname]->setReformator(&name_reformator);
	patient_field[lname]->setReformator(&name_reformator);

	this->patientTypeCombo = patientType;

	this->sexCombo = sexCombo;

	this->cityCodeLabel = cityCodeLabel;

	for (int i = 0; i < patient_field.size(); i++)
		patient_field[i]->setOberver(this);

	sexCombo->setObserver(this);
}


void PatientDialogPresenter::setPatientToView(const Patient& patient)
{
	if (patient.type == 1)
	{
		patientTypeCombo->setIndex(0);
		EgnTypeDialog();
	}
	else
	{
		patientTypeCombo->setIndex(1);
		Ln4TypeDialog();
	}

	patient_field[id]->setFieldText(patient.id);
	patient_field[birthdate]->setFieldText(patient.birth);
	sexCombo->setIndex(patient.sex);
	patient_field[fname]->setFieldText(patient.FirstName);
	patient_field[mname]->setFieldText(patient.MiddleName);
	patient_field[lname]->setFieldText(patient.LastName);
	patient_field[city]->setFieldText(patient.city);
	patient_field[address]->setFieldText(patient.address);
	patient_field[hirbno]->setFieldText(patient.HIRBNo);
	patient_field[phone]->setFieldText(patient.phone);
	
	setCityCodesLabel();

	allergies = patient.allergies;
	currentDiseases = patient.currentDiseases;
	pastDiseases = patient.pastDiseases;
}


Patient PatientDialogPresenter::getPatientFromView()
{
	Patient patient;

	if (!patientTypeCombo->getIndex()) patient.type = 1;
	else patient.type = 2;
	patient.id = patient_field[id]->getText();
	patient.sex = sexCombo->getIndex();
	patient.birth = patient_field[birthdate]->getText();
	patient.FirstName = patient_field[fname]->getText();
	patient.MiddleName = patient_field[mname]->getText();
	patient.LastName = patient_field[lname]->getText();
	patient.city = patient_field[city]->getText();
	patient.address = patient_field[address]->getText();
	patient.HIRBNo = patient_field[hirbno]->getText();
	patient.phone = patient_field[phone]->getText();
	patient.allergies = allergies;
	patient.currentDiseases = currentDiseases;
	patient.pastDiseases = pastDiseases;

	return patient;
}