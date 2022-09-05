#include "PerscriptionPresenter.h"
#include "MedicationPresenter.h"

PerscriptionPresenter::PerscriptionPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId) :
	TabInstance(tabView, TabType::Perscription, patient), view(tabView->perscriptionView())
{
}

void PerscriptionPresenter::addPressed()
{
	MedicationPresenter p;
	p.openDialog();
}

void PerscriptionPresenter::setDataToView()
{
	view->setPresenter(this);
	view->setPatient(*patient.get(), Date::currentDate());
}

long long PerscriptionPresenter::rowID() const
{
	return 0;
}

bool PerscriptionPresenter::save()
{
	return true;
}

void PerscriptionPresenter::print()
{
}

bool PerscriptionPresenter::isNew()
{
	return true;
}

TabName PerscriptionPresenter::getTabName()
{
	return TabName();
}
