#include "PracticeSettingsPresenter.h"

PracticeSettingsPresenter::PracticeSettingsPresenter()
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
}

void PracticeSettingsPresenter::setPractice(const Practice& practice)
{
	if (view == nullptr)
		return;

	view->setPractice(practice);
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

	return true;
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
