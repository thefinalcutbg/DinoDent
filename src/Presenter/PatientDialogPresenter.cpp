#include "PatientDialogPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbPatient.h"
#include "Printer/Print.h"
#include "Model/User.h"

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
	teeth_notes(patient.teethNotes),
	patientNotes(patient.patientNotes),
	dialogTitle("Редактиране на данни на пациента")
{
	if (patient.type == 1) {
		m_patient->sex = Patient::getSexFromEgn(patient.id);
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

	if (!m_patient.has_value()) return;

	view->setPatient(*m_patient);
	view->setEditMode(true);
	m_patient.reset();

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

	if (!view->inputFieldsAreValid()) return;

	m_patient = getPatientFromView();
	
	if (rowid == 0) {
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

void PatientDialogPresenter::searchDbForPatient(int type, const std::string& id)
{
	Patient patient = DbPatient::get(id, type);

	if (patient.rowid){
		rowid = patient.rowid;
		medStats = patient.medStats;
		allergies = patient.allergies;
		teeth_notes = patient.teethNotes;
		patientNotes = patient.patientNotes;
	}
	else {
		//very important to set rowid to 0 (see getPatientFromView())
		rowid = 0;

		if (User::hasNhifContract() &&
			User::settings().getHirbNoAuto &&
			patient.type < 3
			) 
		{
			checkHirbno();
		}
	}
		
	view->setPatient(patient);
	
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

	//if the patient has rowid, set the cached data
	if (rowid) { 
		patient.medStats = medStats;
		patient.allergies = allergies;
		patient.rowid = rowid;
		patient.insuranceStatus = insurance_status;
		patient.PISHistory = PISHistory;
		patient.HISHistory = HISHistory;
		patient.teethNotes = teeth_notes;
		patient.patientNotes = patientNotes;
	}
	return patient;
}
