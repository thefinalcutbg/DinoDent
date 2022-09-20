#include "SettingsMainPresenter.h"
#include "Database/DbPractice.h"
#include "Database/DbDoctor.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"

SettingsMainPresenter::SettingsMainPresenter()
{

}

void SettingsMainPresenter::setView(ISettingsDialog* view)
{
	this->view = view;


	m_practiceDoctorPresenter.setDoctorsList(DbPractice::getDoctors(User::practice().rziCode));

	m_practicePresenter.setView(view->practiceView());
	m_priceListPresenter.setView(view->priceListView());
	m_practiceDoctorPresenter.setView(view->practiceDoctorView());

	m_practicePresenter.setPractice(User::practice());
	m_priceListPresenter.setPriceList(User::practice().priceList);

	view->setSettings(User::practice().settings);
}

void SettingsMainPresenter::okPressed()
{
	if (applyChanges())
	{
		view->closeDialog();
	}
}

bool SettingsMainPresenter::applyChanges()
{
	if (!m_practicePresenter.isValid()){
		view->focusTab(SettingsTab::Practice);
		return false;
	}

	if (!m_practiceDoctorPresenter.isValid())
	{
		view->focusTab(SettingsTab::Doctor);
		ModalDialogBuilder::showError(u8"Практиката трябва да има поне един администратор!");
		return false;
	}

	auto practice = m_practicePresenter.getPractice();
	practice.priceList = m_priceListPresenter.priceList();
	practice.settings = view->getSettings();
	auto doctorsList = m_practiceDoctorPresenter.getDoctorsList();
	
	
	DbPractice::updatePractice(practice, User::practice().rziCode);
	DbPractice::updatePriceList(practice.priceList, practice.rziCode);
	DbPractice::setDoctorsPracticeList(doctorsList, practice.rziCode);

	//the specialty could be changed!
	User::refereshPracticeDoctor();
	User::setCurrentPractice(practice);


	
	return true;
}
