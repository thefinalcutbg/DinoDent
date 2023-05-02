#include "PatientDialogPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbPatient.h"
#include "View/Printer.h"
#include "Database/DbNotes.h"
PatientDialogPresenter::PatientDialogPresenter() :
	view(nullptr)
{}

PatientDialogPresenter::PatientDialogPresenter(const Patient& patient) :
	m_patient(patient),
	rowid(patient.rowid),
	view(nullptr),
	insurance_status(patient.insuranceStatus)
{}

std::optional<Patient> PatientDialogPresenter::open()
{
	ModalDialogBuilder::openDialog(this);
	return m_patient;
}

void PatientDialogPresenter::setView(IPatientDialog* view)
{
	this->view = view;

	birth_validator.setMaxDate(Date::currentDate());
	birth_validator.setMaxErrorMsg("Невалидна рожденна дата");
	birth_validator.setMinDate(Date(2, 1, 1900));
	birth_validator.setMinErrorMsg("Невалидна рожденна дата");

	view->lineEdit(city)->setInputValidator(&city_validator);
	view->lineEdit(hirbno)->setInputValidator(&hirb_validator);
	view->lineEdit(fname)->setInputValidator(&name_validator);
	view->lineEdit(lname)->setInputValidator(&name_validator);
	view->dateEdit()->setInputValidator(&birth_validator);
	changePatientType(1);

	if (m_patient.has_value())
	{
		setPatientToView(m_patient.value());
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

void PatientDialogPresenter::checkHirbno()
{
	auto p = view->getPatient();
	
	hirbnoHandler.sendRequest(p.type, p.id, 
		[=](auto hirbno) {if (this)this->setHirbno(hirbno);}
	);

}


void PatientDialogPresenter::accept()
{

	if (!viewIsValid()) return;

	m_patient = getPatientFromView();
	
	bool success{ false };

	if (rowid == 0) {
		rowid = m_patient->rowid;
		m_patient->rowid = DbPatient::insert(m_patient.value());

		if (!m_patient->rowid) m_patient.reset();
	}
	else
	{
		if (!DbPatient::update(m_patient.value())) m_patient.reset();
	}

	if (!m_patient) {
		ModalDialogBuilder::showError("Неуспешно записване в базата данни");
	}
	
	view->close();
}

void PatientDialogPresenter::searchDbForPatient(int type)
{
	
	std::string patientId = view->lineEdit(id)->getText();

	Patient patient = DbPatient::get(patientId, type);

	medStats = patient.medStats;

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


void PatientDialogPresenter::setPatientToView(const Patient& patient)
{
	changePatientType(patient.type);

	view->setPatient(patient);
	
	view->lineEdit(id)->validateInput();

	medStats = patient.medStats;

}

bool PatientDialogPresenter::viewIsValid()
{
	auto inputIsValid = [](AbstractUIElement* uiElement)
	{
		uiElement->validateInput();
		if (!uiElement->isValid())
		{
			uiElement->setFocus();
			return false;
		}
		return true;
	};

	for (int i = 0; i < 8; i++)
	{
		if (!inputIsValid(view->lineEdit(static_cast<PatientField>(i)))) return false;
	}

	if (!inputIsValid(view->dateEdit())) return false;

	return true;
}

void PatientDialogPresenter::setHirbno(const std::string& hirbno)
{
	if(hirbno.empty()){
		ModalDialogBuilder::showMessage("Не е намерена активна здравна книжка");
		return;
	}

	view->setHirbno(hirbno);
}


Patient PatientDialogPresenter::getPatientFromView()
{
	Patient patient = view->getPatient();

	patient.medStats = medStats;
	patient.rowid = rowid;
	patient.insuranceStatus = insurance_status;
	return patient;
}