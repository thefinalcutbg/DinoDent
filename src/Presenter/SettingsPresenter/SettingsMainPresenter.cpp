﻿#include "SettingsMainPresenter.h"
#include "Database/DbLogin.h"
#include "Model/User/UserManager.h"
#include "View/ModalDialogBuilder.h"

SettingsMainPresenter::SettingsMainPresenter()
{

}

void SettingsMainPresenter::setView(ISettingsDialog* view)
{
	this->view = view;

	DbLogin db;

	m_practiceDoctorPresenter.setDoctorsList(db.getDoctors(UserManager::currentUser().practice.rziCode));

	m_practicePresenter.setView(view->practiceView());
	m_priceListPresenter.setView(view->priceListView());
	m_practiceDoctorPresenter.setView(view->practiceDoctorView());

	m_practicePresenter.setPractice(UserManager::currentUser().practice);
	m_priceListPresenter.setPriceList(UserManager::currentUser().practice.priceList);
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
	auto doctorsList = m_practiceDoctorPresenter.getDoctorsList();

	DbLogin db;
	db.updatePractice(practice, UserManager::currentUser().practice.rziCode);
	db.updatePriceList(practice.priceList, practice.rziCode);
	db.setDoctorsPracticeList(doctorsList, practice.rziCode);

	UserManager::setCurrentPractice(practice);

	return true;
}