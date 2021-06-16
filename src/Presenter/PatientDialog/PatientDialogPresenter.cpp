#include "PatientDialogPresenter.h"
#include "View/ModalDialogBuilder.h"

PatientDialogPresenter::PatientDialogPresenter() :
	view(nullptr),
	new_patient(true)
{}

PatientDialogPresenter::PatientDialogPresenter(const Patient& patient) :
	_patient(patient),
	view(nullptr),
	new_patient(false)
{}


std::optional<Patient> PatientDialogPresenter::open()
{
	ModalDialogBuilder::openDialog(this);
	return _patient;
}

void PatientDialogPresenter::changePatientType(int index)
{
	switch (index)
	{
	case 1:
		view->setLn4View(false);

		view->lineEdit(id)->set_Validator(&egn_validator);
		view->lineEdit(birthdate)->set_Validator(nullptr);
		view->lineEdit(mname)->set_Validator(&name_validator);
		view->lineEdit(id)->forceValidate();
		view->resetFields();
		break;
	case 2:
		view->setLn4View(true);

		view->lineEdit(id)->set_Validator(&ln4_validator);
		view->lineEdit(birthdate)->set_Validator(&birth_validator);
		view->lineEdit(mname)->set_Validator(nullptr);
		view->lineEdit(id)->forceValidate();
		view->resetFields();
		break;
	default:
		break;

	}
}



void PatientDialogPresenter::accept()
{
	for(int i = 0; i < 9; i++)
	{	
		auto* field = view->lineEdit(static_cast<PatientField>(i));

		field->forceValidate();
		if (!field->isValid())
		{
			field->setFocusAndSelectAll();
			return;
		}
	}

	_patient = getPatientFromView();

	if(new_patient) database.insert(_patient.value());
	else database.update(_patient.value());
	
	view->close();
}

void PatientDialogPresenter::setView(IPatientDialog* view)
{
	this->view = view;

	view->lineEdit(city)->set_Validator(&city_validator);
	view->lineEdit(hirbno)->set_Validator(&hirb_validator);
	view->lineEdit(fname)->set_Validator(&name_validator);
	view->lineEdit(lname)->set_Validator(&name_validator);
	changePatientType(1);

	if (_patient.has_value())
	{
		setPatientToView(_patient.value());
		view->setEditMode(true);
	}

}


void PatientDialogPresenter::searchDbForPatient()
{
	
	std::string lineEditID = view->lineEdit(id)->getText();

	Patient patient = database.getPatient(lineEditID);

	allergies = patient.allergies;
	pastDiseases = patient.pastDiseases;
	currentDiseases = patient.currentDiseases;

	if (patient.id == "")
	{
		new_patient = true;
		patient.id = lineEditID;

		if (patient.type == 1)
		{
			patient.birth = Date::GetDateFromEgn(patient.id);
			patient.sex = Patient::getSexFromEgn(lineEditID);
		}
	}
	else
		new_patient = false;

	setPatientToView(patient);
	
}

void PatientDialogPresenter::cityChanged()
{
	view->lineEdit(city)->isValid() ?

		view->setCodeInfo(city_code.getLabel(view->lineEdit(city)->getText()))
		:
		view->setCodeInfo("");
}



void PatientDialogPresenter::setPatientToView(const Patient& patient)
{

	changePatientType(patient.type);

	view->setPatient(patient);
	
	view->lineEdit(id)->forceValidate();

	cityChanged();

	allergies = patient.allergies;
	currentDiseases = patient.currentDiseases;
	pastDiseases = patient.pastDiseases;
}


Patient PatientDialogPresenter::getPatientFromView()
{
	Patient patient = view->getPatient();;

	patient.allergies = allergies;
	patient.currentDiseases = currentDiseases;
	patient.pastDiseases = pastDiseases;

	return patient;
}