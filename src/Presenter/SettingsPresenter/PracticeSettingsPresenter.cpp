#include "PracticeSettingsPresenter.h"
#include "Model/User/UserManager.h"
#include "Database/DbLogin.h"

PracticeSettingsPresenter::PracticeSettingsPresenter() 
{

}

void PracticeSettingsPresenter::setView(IPracticeSettings* view)
{
	this->view = view;
	view->setPractice(UserManager::currentUser().practice);
}

void PracticeSettingsPresenter::applyChanges()
{
	auto p = view->getPractice();
	DbLogin db;
	p.priceList = UserManager::currentUser().practice.priceList;
	db.updatePractice(p, UserManager::currentUser().practice.rziCode);
	UserManager::setCurrentPractice(p);
}
