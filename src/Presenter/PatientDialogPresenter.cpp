#include "PatientDialogPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbPatient.h"
#include "View/Printer.h"

PatientDialogPresenter::PatientDialogPresenter(std::string dialogTitle) :
	view(nullptr), dialogTitle(dialogTitle)
{}

PatientDialogPresenter::PatientDialogPresenter(const Patient& patient) :
	m_patient(patient),
    view(nullptr),
    insurance_status(patient.insuranceStatus),
    PISHistory(patient.PISHistory),
    HISHistory(patient.HISHistory),
    rowid(patient.rowid),
	medStats(patient.medStats),
	allergies(patient.allergies),
	dialogTitle("Редактиране на данни на пациента")
{
	if (patient.type == 1) {
		m_patient->birth = Date::getBirthdateFromEgn(patient.id);
	}
}

std::optional<Patient> PatientDialogPresenter::open()
{
    ModalDialogBuilder::openDialog(*this);
    return m_patient;
}

void PatientDialogPresenter::setView(IPatientDialog* view)
{
	this->view = view;

	view->setTitle(dialogTitle);

	birth_validator.setMaxDate(Date::currentDate());
	birth_validator.setMaxErrorMsg("Невалидна рождена дата");
	birth_validator.setMinDate(Date(2, 1, 1900));
	birth_validator.setMinErrorMsg("Невалидна рождена дата");

	view->lineEdit(city)->setInputValidator(&city_validator);
	view->lineEdit(hirbno)->setInputValidator(&hirb_validator);
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
		view->setType(Patient::EGN);

		view->lineEdit(id)->setInputValidator(&egn_validator);
		view->lineEdit(fname)->setInputValidator(&name_validator);
		view->lineEdit(mname)->setInputValidator(&cyrillic_validator);
		view->lineEdit(lname)->setInputValidator(&name_validator);
		view->lineEdit(address)->setInputValidator(nullptr);
		view->lineEdit(foreign_city)->setInputValidator(nullptr);
		view->lineEdit(id)->validateInput();

		view->resetFields();
		break;

	case 2:
		view->setType(Patient::LNCH);

		view->lineEdit(id)->setInputValidator(&ln4_validator);
		view->lineEdit(fname)->setInputValidator(&name_validator);
		view->lineEdit(mname)->setInputValidator(&cyrillic_validator);
		view->lineEdit(lname)->setInputValidator(&name_validator);
		view->lineEdit(address)->setInputValidator(nullptr);
		view->lineEdit(foreign_city)->setInputValidator(nullptr);
		view->lineEdit(id)->validateInput();;
		view->resetFields();
		break;
	case 3:
		view->setType(Patient::SSN);

		view->lineEdit(id)->setInputValidator(&ssn_validator);
		view->lineEdit(fname)->setInputValidator(&name_validator);
		view->lineEdit(mname)->setInputValidator(&cyrillic_validator);
		view->lineEdit(lname)->setInputValidator(&name_validator);
		view->lineEdit(address)->setInputValidator(nullptr);
		view->lineEdit(foreign_city)->setInputValidator(nullptr);
		view->lineEdit(id)->validateInput();
		view->resetFields();
		break;
	case 4:
		view->setType(Patient::EU);

		view->lineEdit(id)->setInputValidator(&notEmpty_validator);
		view->lineEdit(fname)->setInputValidator(&name_validator);
		view->lineEdit(mname)->setInputValidator(&cyrillic_validator);
		view->lineEdit(lname)->setInputValidator(&name_validator);
		view->lineEdit(address)->setInputValidator(nullptr);
		view->lineEdit(foreign_city)->setInputValidator(&notEmpty_validator);
		view->lineEdit(id)->validateInput();
		view->resetFields();

		break;
	default:
		break;

	}
}

void PatientDialogPresenter::checkHirbno()
{
	auto p = view->getPatient();
	
	hirbnoHandler.sendRequest(p, 
        [=, this](auto hirbno) { this->setHirbno(hirbno);}
	);

}


void PatientDialogPresenter::accept()
{

	if (!viewIsValid()) return;

	m_patient = getPatientFromView();
	
	if (rowid == 0) {
		rowid = m_patient->rowid;
		m_patient->rowid = DbPatient::insert(m_patient.value());

		if (!m_patient->rowid) m_patient.reset();
	}
	else
	{
		if (!DbPatient::update(m_patient.value())) {
			m_patient.reset();
			ModalDialogBuilder::showError("Неуспешно записване в базата данни");
		}
	}
	
	view->close();
}

void PatientDialogPresenter::searchDbForPatient(int type)
{
	std::string patientId = view->lineEdit(id)->getText();

	Patient patient = DbPatient::get(patientId, type);

	medStats = patient.medStats;
	allergies = patient.allergies;

	if (patient.rowid){
		rowid = patient.rowid;
	}
		
	setPatientToView(patient);
	
}


void PatientDialogPresenter::setPatientToView(const Patient& patient)
{
	changePatientType(patient.type);

	view->setPatient(patient);
	
	view->lineEdit(id)->validateInput();

}

bool PatientDialogPresenter::viewIsValid()
{
	auto inputIsValid = [](AbstractUIElement* uiElement)
	{
		uiElement->validateInput();
		if (!uiElement->isValid())
		{
            uiElement->set_focus();
			return false;
		}
		return true;
	};

	for (int i = 0; i < PatientField::size; i++)
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
	patient.allergies = allergies;
	patient.rowid = rowid;
	patient.insuranceStatus = insurance_status;
	patient.PISHistory = PISHistory;
	patient.HISHistory = HISHistory;
	return patient;
}
