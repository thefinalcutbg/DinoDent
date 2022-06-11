#include "PatientDialogPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbPatient.h"
#include "Network/PISServ.h"

PatientDialogPresenter::PatientDialogPresenter() :
	view(nullptr)
{}

PatientDialogPresenter::PatientDialogPresenter(const Patient& patient) :
	_patient(patient),
	rowid(patient.rowid),
	view(nullptr)
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

void PatientDialogPresenter::activeHirbnoCheck()
{
	view->disableHirbnoButton(true);

	auto p = view->getPatient();
	
	bool success = PIS::sendRequest(SOAP::activeHIRBNo(p.id, p.type), hirbnoHandler);
	
	if (!success) {
		view->disableHirbnoButton(false);
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
	
	if (rowid == 0) {
		rowid = _patient->rowid;
		_patient->rowid = DbPatient::insert(_patient.value());
	}
	else DbPatient::update(_patient.value());
	
	view->close();
}

void PatientDialogPresenter::searchDbForPatient(int type)
{
	
	std::string patientId = view->lineEdit(id)->getText();

	Patient patient = DbPatient::get(patientId, type);

	allergies = patient.allergies;
	pastDiseases = patient.pastDiseases;
	currentDiseases = patient.currentDiseases;

	if (patient.rowid == 0)
	{
		patient.id = patientId;
		patient.type = type;

		if (patient.type == 1)
		{
			patient.birth = Date::getBirthdateFromEgn(patient.id);
			patient.sex = Patient::getSexFromEgn(patientId);
		}
	}
	else
	{
		rowid = patient.rowid;
	}
		

	setPatientToView(patient);
	
}

void PatientDialogPresenter::cityChanged()
{
	view->lineEdit(city)->isValid() ?

		view->setCodeInfo(CityCode::getLabel(view->lineEdit(city)->getText()))
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

void PatientDialogPresenter::setHirbno(const std::string& hirbno)
{
	view->disableHirbnoButton(false);

	if(hirbno.empty()){
		ModalDialogBuilder::showMessage(u8"Не е намерена активна здравна книжка");
		return;
	}

	view->setHirbno(hirbno);
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
	patient.rowid = rowid;

	return patient;
}