#include "PracticeSettingsPresenter.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/DoctorDialogPresenter.h"
#include "Database/DbPractice.h"

PracticeSettingsPresenter::PracticeSettingsPresenter(std::string practiceRzi) :
	initialRzi(practiceRzi), 
	m_doctorsList{ DbPractice::getDoctors(practiceRzi) }
{
	m_dateValidator.setMaxDate(Date::currentDate());
	m_dateValidator.setMinDate(Date{ 2,1,1900 });
}

void PracticeSettingsPresenter::setView(IPracticeSettings* view)
{
	this->view = view;

	view->setPresenter(this);

	view->lineEdit(PracticeTextFields::Name)->setInputValidator(&m_notEmptyValidator);
	view->lineEdit(PracticeTextFields::Bulstat)->setInputValidator(&m_notEmptyValidator);
	view->lineEdit(PracticeTextFields::RZI)->setInputValidator(&m_rziValidator);
	view->lineEdit(PracticeTextFields::Password)->setInputValidator(&m_notEmptyValidator);
	view->lineEdit(PracticeTextFields::ActivityAddress)->setInputValidator(&m_notEmptyValidator);
	view->lineEdit(PracticeTextFields::Address)->setInputValidator(&m_notEmptyValidator);

	view->setPresenter(this);

	if (initialRzi.size())(view->hidePassword());
	view->setPractice(DbPractice::getPractice(initialRzi));
	view->setDoctorList(m_doctorsList);

	view->setDoctorList(m_doctorsList);
}


Practice PracticeSettingsPresenter::getPractice()
{
	return view->getPractice();
}

bool PracticeSettingsPresenter::isValid()
{

	AbstractUIElement* uiFields[10]
	{
		view->lineEdit(PracticeTextFields::Name),
		view->lineEdit(PracticeTextFields::RZI),
		view->lineEdit(PracticeTextFields::Bulstat),
		view->lineEdit(PracticeTextFields::Address),
		view->lineEdit(PracticeTextFields::ActivityAddress),
		view->lineEdit(PracticeTextFields::Password),
		view->lineEdit(PracticeTextFields::VAT),
		view->lineEdit(PracticeTextFields::NZOKContract),
		view->lineEdit(PracticeTextFields::NZOKShortName),
		view->dateEdit()
	};

	for (auto field : uiFields)
	{
		field->validateInput();

		if(!field->isValid()) {
			field->setFocus();
			return false;
		}
		
	}

	//checks for rzi uniqueness
	auto currentRzi = view->lineEdit(PracticeTextFields::RZI)->getText();

	for (auto& p : DbPractice::getPracticeList()) {

		if (p.rzi == initialRzi) continue;

		if (p.rzi == currentRzi) {
			ModalDialogBuilder::showMessage(u8"Практика с такъв номер вече съществува");
			return false;
		}
	}

	for (auto& doctor : m_doctorsList) {
		if (doctor.admin) return true;
	}

	ModalDialogBuilder::showError(u8"Практиката трябва да има поне един администратор");

	return false;
}

void PracticeSettingsPresenter::nzokContractEnabled(bool enabled)
{
	AbstractUIElement* validatableElements[3]
	{
		view->lineEdit(PracticeTextFields::NZOKContract),
		view->lineEdit(PracticeTextFields::NZOKShortName),
		view->dateEdit()
	};

	if(enabled){

		validatableElements[0]->setInputValidator(&m_contractValidator);
		validatableElements[1]->setInputValidator(&m_notEmptyValidator);
		validatableElements[2]->setInputValidator(&m_dateValidator);

	}
	else {
		for (auto& e : validatableElements){
			e->setInputValidator(nullptr);
		}
	}
	
}

void PracticeSettingsPresenter::vatEnabled(bool enabled)
{
	view->lineEdit(PracticeTextFields::VAT)->setInputValidator(
		enabled ?
		&m_notEmptyValidator
		:
		nullptr
	);
}

void PracticeSettingsPresenter::addDoctor()
{
	DoctorDialogPresenter p;

	auto doctor = p.open();

	if (!doctor.has_value()) return;

	for (auto& docPair : m_doctorsList)
	{
		if (docPair.lpk == doctor.value().LPK)
		{
			ModalDialogBuilder::showError(u8"Този доктор вече е прибавен към практиката");
			return;
		}
	}

	m_doctorsList.emplace_back(PracticeDoctor{ doctor.value().LPK, doctor.value().getFullName(), false });

	if (m_doctorsList.size() == 1) {
		m_doctorsList[0].admin = true;
	}

	view->setDoctorList(m_doctorsList);


}

void PracticeSettingsPresenter::deleteDoctor()
{
	if (m_currentIndex < 0) return;

	if (User::isCurrentUser(m_doctorsList[m_currentIndex].lpk))
	{
		ModalDialogBuilder::showError(u8"Не можете да изтриете профила от който сте влезли в момента");
		return;
	}

	m_doctorsList.erase(m_doctorsList.begin() + m_currentIndex);
	view->setDoctorList(m_doctorsList);

}

void PracticeSettingsPresenter::indexChanged(int index)
{
	m_currentIndex = index;

	if (m_currentIndex < 0) return;

	view->setDoctorProperties(m_doctorsList[index].admin, m_doctorsList[index].specialty);

}

void PracticeSettingsPresenter::setAdminPrivilege(bool admin)
{
	m_doctorsList[m_currentIndex].admin = admin;

	view->replaceCurrentItem(m_doctorsList[m_currentIndex]);
}

void PracticeSettingsPresenter::setDoctorNhifSpecialty(NhifSpecialty spec)
{
	m_doctorsList[m_currentIndex].specialty = spec;
}

std::vector<PracticeDoctor> PracticeSettingsPresenter::getDoctorsList()
{
	return m_doctorsList;
}
