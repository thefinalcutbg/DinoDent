#include "PracticeDoctorSettingsPresenter.h"
#include "Database/DbLogin.h"
#include "Model/User/UserManager.h"
#include "View/ModalDialogBuilder.h"
#include "Presenter/DoctorDialogPresenter/DoctorDialogPresenter.h"
#include <algorithm>

PracticeDoctorSettingsPresenter::PracticeDoctorSettingsPresenter()
{
	DbLogin db;

	m_doctorsList = db.getDoctors(UserManager::currentUser().practice.rziCode);;

}

void PracticeDoctorSettingsPresenter::setDoctorsList(const std::vector<PracticeDoctor>& doctorsList)
{
	m_doctorsList = doctorsList;
}

void PracticeDoctorSettingsPresenter::setView(IPracticeDoctorSettings* view)
{
	this->view = view;
	view->setPresenter(this);
	view->setDoctorList(m_doctorsList);
}

void PracticeDoctorSettingsPresenter::addDoctor()
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

	view->setDoctorList(m_doctorsList);


}

void PracticeDoctorSettingsPresenter::deleteDoctor()
{
	if(m_currentIndex < 0) return;

	if (UserManager::instance().isCurrentUser(m_doctorsList[m_currentIndex].lpk))
	{
		ModalDialogBuilder::showError(u8"Не можете да изтриете профилът от който сте влезли в момента");
		return;
	}

	m_doctorsList.erase(m_doctorsList.begin() + m_currentIndex);
	view->setDoctorList(m_doctorsList);

}

void PracticeDoctorSettingsPresenter::applyChanges()
{
	DbLogin db;
	db.setDoctorsPracticeList(m_doctorsList, UserManager::currentUser().practice.rziCode);
}

void PracticeDoctorSettingsPresenter::indexChanged(int index)
{
	 m_currentIndex = index;

	 if (m_currentIndex < 0)
	 {
		 view->setAdminCheckbox(AdminStatus::Hidden);
		 return;
	 }

	 m_doctorsList[index].admin ?
		 view->setAdminCheckbox(AdminStatus::Checked)
		 :
		 view->setAdminCheckbox(AdminStatus::Unchecked);
}

void PracticeDoctorSettingsPresenter::setAdminPrivilege(bool admin)
{
	m_doctorsList[m_currentIndex].admin = admin;

	view->replaceCurrentItem(m_doctorsList[m_currentIndex]);
}

bool PracticeDoctorSettingsPresenter::isValid()
{
	return std::count_if(
				m_doctorsList.begin(),
				m_doctorsList.end(),
				[](const PracticeDoctor& doc) {
					return doc.admin;
				}
	);
}

std::vector<PracticeDoctor> PracticeDoctorSettingsPresenter::getDoctorsList()
{
	return m_doctorsList;
}
