#include "SettingsMainPresenter.h"
#include "Database/DbPractice.h"
#include "Database/DbDoctor.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbUpdateStatus.h"

SettingsMainPresenter::SettingsMainPresenter() :
	m_practicePresenter(User::practice().rziCode)
{

}

void SettingsMainPresenter::setView(ISettingsDialog* view)
{
	this->view = view;

	m_practicePresenter.setView(view->practiceView());

	view->setSettings(User::practice().settings);
	setUpdateLabels();
}

void SettingsMainPresenter::okPressed()
{
	if (applyChanges())
	{
		view->closeDialog();
	}
}

void SettingsMainPresenter::setUpdateLabels() {

	view->setUpdateDate(
		DynamicNum::Medication,
		DbUpdateStatus::lastUpdated(DynamicNum::Medication)
	);
}

void SettingsMainPresenter::updateMedications()
{
	med_update.setCallback
	(
		[&](bool success) { if (success) setUpdateLabels();}
	);

	med_update.update();
}

bool SettingsMainPresenter::applyChanges()
{
	if (!m_practicePresenter.isValid()){
		view->focusTab(SettingsTab::Practice);
		return false;
	}

	auto practice = m_practicePresenter.getPractice();

	if (
		practice.rziCode != User::practice().rziCode  //if the practice rzi has been changed
		&&
		DbPractice::practiceExists(practice.rziCode) //but the new rzi already exists in the db
	) 
	{
		view->focusTab(SettingsTab::Practice);
		ModalDialogBuilder::showError("Практика с такъв РЗИ номер вече съществува");
		return false;
	}

	auto doctorsList = m_practicePresenter.getDoctorsList();
	practice.settings = view->getSettings();

	
	
	DbPractice::updatePractice(practice, User::practice().rziCode);
	//DbPractice::updatePriceList(practice.priceList, practice.rziCode);
	DbPractice::setDoctorsPracticeList(doctorsList, practice.rziCode);

	//the specialty could be changed!
	User::refereshPracticeDoctor();
	User::setCurrentPractice(practice);

	return true;
}
