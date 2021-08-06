#include "PatientDialogPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Model/User/CurrentUser.h"

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

void PatientDialogPresenter::setView(IPatientDialog* view)
{
	this->view = view;

	birth_validator.setMaxDate(Date::currentDate());
	birth_validator.setMaxErrorMsg(u8"Невалидна рожденна дата");
	birth_validator.setMinDate(Date(2, 1, 1900));
	birth_validator.setMinErrorMsg(u8"Невалидна рожденна дата");

	view->lineEdit(city)->setInputValidator(&city_validator);
	view->lineEdit(hirbno)->setInputValidator(&hirb_validator);
	view->lineEdit(fname)->setInputValidator(&name_validator);
	view->lineEdit(lname)->setInputValidator(&name_validator);
	view->dateEdit()->setInputValidator(&birth_validator);
	changePatientType(1);

	if (_patient.has_value())
	{
		setPatientToView(_patient.value());
		view->setEditMode(true);
	}

}

void PatientDialogPresenter::changePatientType(int index)
{
	switch (index)
	{
	case 1:
		view->setLn4View(false);

		view->lineEdit(id)->setInputValidator(&egn_validator);
		view->lineEdit(mname)->setInputValidator(&name_validator);
		view->lineEdit(id)->validateInput();
		view->resetFields();
		break;
	case 2:
		view->setLn4View(true);

		view->lineEdit(id)->setInputValidator(&ln4_validator);
		view->lineEdit(mname)->setInputValidator(&cyrillic_validator);
		view->lineEdit(id)->validateInput();;
		view->resetFields();
		break;
	default:
		break;

	}
}

void PatientDialogPresenter::accept()
{

	for(int i = 0; i < 8; i++)
	{	
		if (!inputIsValid(view->lineEdit(static_cast<PatientField>(i)))) return;
	}

	if (!inputIsValid(view->dateEdit())) return;


	_patient = getPatientFromView();

	if(new_patient) amb_db.insert(_patient.value());
	else amb_db.update(_patient.value());
	
	view->close();
}

void PatientDialogPresenter::searchDbForPatient(int type)
{
	
	std::string lineEditID = view->lineEdit(id)->getText();

	Patient patient = amb_db.getPatient(lineEditID);

	allergies = patient.allergies;
	pastDiseases = patient.pastDiseases;
	currentDiseases = patient.currentDiseases;

	if (patient.id.empty())
	{
		new_patient = true;
		patient.id = lineEditID;
		patient.type = type;
		patient.city = CurrentUser::instance().practice_address;

		if (patient.type == 1)
		{
			patient.birth = Date::getBirthdateFromEgn(patient.id);
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
	
	view->lineEdit(id)->validateInput();

	cityChanged();

	allergies = patient.allergies;
	currentDiseases = patient.currentDiseases;
	pastDiseases = patient.pastDiseases;
}

bool PatientDialogPresenter::inputIsValid(AbstractUIElement* uiElement)
{
	uiElement->validateInput();
	if (!uiElement->isValid())
	{
		uiElement->setFocus();
		return false;
	}
	return true;
}

Patient PatientDialogPresenter::getPatientFromView()
{
	Patient patient = view->getPatient();;

	patient.allergies = allergies;
	patient.currentDiseases = currentDiseases;
	patient.pastDiseases = pastDiseases;

	return patient;
}