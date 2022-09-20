#include "PerscriptionPresenter.h"
#include "MedicationPresenter.h"

PerscriptionPresenter::PerscriptionPresenter(ITabView* tabView, TabPresenter* tabPresenter, std::shared_ptr<Patient> patient, long long rowId) :
	TabInstance(tabView, TabType::Perscription, patient), view(tabView->perscriptionView())
{
}

void PerscriptionPresenter::addPressed()
{

	MedicationPresenter p;

	auto result = p.openDialog();

	if (!result) return;

	m_perscription.medicationGroup.push_back(result.value());
	
	view->setMedicationList(m_perscription.getMedList());
}

void PerscriptionPresenter::editPressed(int idx)
{
	if (idx == -1) return;

	MedicationPresenter p(m_perscription.medicationGroup[idx]);

	auto result = p.openDialog();

	if (!result) return;

	m_perscription.medicationGroup[idx] = result.value();

	view->setMedicationList(m_perscription.getMedList());

}

void PerscriptionPresenter::deletePressed(int idx)
{
	if (idx == -1) return;

	auto& medVec = m_perscription.medicationGroup;

	medVec.erase(medVec.begin() + idx);

	view->setMedicationList(m_perscription.getMedList());
}

void PerscriptionPresenter::supplementsChanged(const std::string& s)
{
	m_perscription.supplements = s;
}

void PerscriptionPresenter::dispensationChanged(const Dispensation& d)
{
	m_perscription.dispensation = d;
}

void PerscriptionPresenter::setDataToView()
{
	view->setPresenter(this);
	view->setPatient(*patient.get(), Date::currentDate());
	view->setDispensation(m_perscription.dispensation);
	view->setSupplements(m_perscription.supplements);
	view->setMedicationList(m_perscription.getMedList());
}

long long PerscriptionPresenter::rowID() const
{
	return 0;
}

bool PerscriptionPresenter::save()
{
	return true;
}

void PerscriptionPresenter::print() {}

bool PerscriptionPresenter::isNew()
{
	return m_perscription.rowid == 0;
}

TabName PerscriptionPresenter::getTabName()
{
	TabName result;
	result.header = isNew() ? u8"Нова рецепта" : u8"Рецепта";
	
	result.header += m_perscription.NRN.empty() ?
		 u8"(неизпратена)" : m_perscription.NRN;
	
	result.footer = patient->firstLastName();
	result.nzok = false;

	return result;
}
