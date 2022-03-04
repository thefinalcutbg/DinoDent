#include "SettingsMainPresenter.h"
#include "Database/DbLogin.h"
#include "Model/User/UserManager.h"

SettingsMainPresenter::SettingsMainPresenter()
{

}

void SettingsMainPresenter::setView(ISettingsDialog* view)
{
	this->view = view;

	m_practicePresenter.setView(view->practiceView());
	m_priceListPresenter.setView(view->priceListView());

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

	Practice p = m_practicePresenter.getPractice();
	p.priceList = m_priceListPresenter.priceList();

	DbLogin db;
	db.updatePractice(p, UserManager::currentUser().practice.rziCode);
	db.updatePriceList(p.priceList, UserManager::currentUser().practice.rziCode);

	UserManager::setCurrentPractice(p);

	return true;
}
