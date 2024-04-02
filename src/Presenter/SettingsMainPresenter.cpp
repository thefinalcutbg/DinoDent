#include "SettingsMainPresenter.h"
#include "Database/DbPractice.h"
#include "Database/DbDoctor.h"
#include "Model/User.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbUpdateStatus.h"
#include "Network/NetworkManager.h"
#include "GlobalSettings.h"
#include "Presenter/DoctorDialogPresenter.h"

SettingsMainPresenter::SettingsMainPresenter() :
	m_doctorsList(DbPractice::getDoctors(User::practice().rziCode))
{

}

void SettingsMainPresenter::setView(ISettingsDialog* view)
{
	this->view = view;

	auto practice = DbPractice::getPractice(User::practice().rziCode);
	auto doctor = *DbDoctor::getDoctor(User::doctor().LPK);

	view->setSettings(practice.settings);
	view->setPkcs11List(GlobalSettings::pkcs11PathList());
	view->setDevBranch(GlobalSettings::getDevBranch());
	view->setDebug(GlobalSettings::showRequestsEnabled(), GlobalSettings::showRepliesEnabled());
	view->setDoctor(doctor);
	view->setPractice(practice);
	view->setDoctorList(m_doctorsList);

	setUpdateLabels();
	
	if (!User::isAdmin()) {
		view->disableTab(SettingsTab::Practice);
		view->disableTab(SettingsTab::Company);
		view->disableTab(SettingsTab::NhifContract);
	}
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

void SettingsMainPresenter::addDoctor()
{
	DoctorDialogPresenter p;

	auto doctor = p.open();

	if (!doctor.has_value()) return;

	for (auto& docPair : m_doctorsList)
	{
		if (docPair.lpk == doctor.value().LPK)
		{
			ModalDialogBuilder::showError("Този лекар вече е прибавен към практиката");
			return;
		}
	}

	m_doctorsList.emplace_back(PracticeDoctor{ doctor.value().LPK, doctor.value().getFullName(), false, NhifSpecialty::None });

	if (m_doctorsList.size() == 1) {
		m_doctorsList[0].admin = true;
	}

	view->setDoctorList(m_doctorsList);
}

void SettingsMainPresenter::removeDoctor()
{
	if (m_currentIndex < 0) return;

	if (User::isCurrentUser(m_doctorsList[m_currentIndex].lpk))
	{
		ModalDialogBuilder::showError("Не можете да изтриете профила от който сте влезли в момента");
		return;
	}

	m_doctorsList.erase(m_doctorsList.begin() + m_currentIndex);
	view->setDoctorList(m_doctorsList);
}

void SettingsMainPresenter::practiceDoctorChanged(int specialtyIdx, bool admin)
{
	m_doctorsList[m_currentIndex].admin = admin;
	m_doctorsList[m_currentIndex].specialty = static_cast<NhifSpecialty>(specialtyIdx);
	view->replaceCurrentItem(m_doctorsList[m_currentIndex]);
}

void SettingsMainPresenter::doctorIndexChanged(int index)
{
	m_currentIndex = index;

	if (m_currentIndex < 0) return;

	view->setDoctorProperties(m_doctorsList[index].admin, m_doctorsList[index].specialty);
}

bool SettingsMainPresenter::applyChanges()
{

	auto practice = view->getPractice();

	//rzi primary key db check
	if (practice.rziCode != User::practice().rziCode &&  //if the practice rzi has been changed
		DbPractice::practiceExists(practice.rziCode) //but the new rzi already exists in the db
	) 
	{
		view->focusTab(SettingsTab::Practice);
		ModalDialogBuilder::showError("Практика с такъв РЗИ номер вече съществува");
		return false;
	}

	auto doctor = view->getDoctor();

	//lpk primary key db check
	if (doctor.LPK != User::doctor().LPK &&
		DbDoctor::suchDoctorExists(doctor.LPK)
	)
	{
		view->focusTab(SettingsTab::Doctor);
		ModalDialogBuilder::showError(
			"Доктор с такъв УИН вече съществува. За да го добавите, " 
			"използвайте \"Добави лекар\" от раздел \"Практика\". "
			"След това излезте от текущия профил и влезте с УИН-а на другия лекар."
		);
		return false;

	}

	if (practice.nhif_contract &&
		practice.nhif_contract->unfavourable &&
		!practice.practice_address.isUnfav() &&
		!ModalDialogBuilder::askDialog(
			"Маркирали сте неблагоприятни условия, но адресът на дейността "
			"не е в списъка с такива. Сигурни ли сте, че искате да продължите?"
		)
		)
	{
		view->focusTab(SettingsTab::NhifContract);
		return false;
	}

	//lame, the state of the practice has to be kept in the presenter
	if (m_doctorsList.size() &&
		m_doctorsList[0].specialty == NhifSpecialty::None &&
		practice.nhif_contract
		)
	{

		bool atLeastOneNhifDoctor{ false };

		for (auto& d : m_doctorsList) {

			if (d.specialty != NhifSpecialty::None) {
				atLeastOneNhifDoctor = true;
				break;
			}
		}

		if (atLeastOneNhifDoctor == false) {

			bool answer = ModalDialogBuilder::askDialog(
				"Практиката има договор с НЗОК, но нито един лекар в нея\n"
				"няма код специалност по НЗОК. Да продължа ли въпреки това?");
			if (!answer) {
				view->focusTab(SettingsTab::Practice);
				return false;
			}

		}

	}

	bool hasAdmin = false;

	for (auto& doctor : m_doctorsList) {
		if (doctor.admin) {
			hasAdmin = true;
			break;
		}
	}

	if (!hasAdmin) {
		view->focusTab(SettingsTab::Practice);
		ModalDialogBuilder::showError("Практиката трябва да има поне един администратор");
		return false;
	}

	practice.settings = view->getSettings();

	DbPractice::updatePractice(practice, User::practice().rziCode);
	DbPractice::setDoctorsPracticeList(m_doctorsList, practice.rziCode);
	DbPractice::updatePracticeSettings(practice.settings, practice.rziCode);
	DbDoctor::updateDoctor(doctor, User::doctor().LPK);

	User::setCurrentPractice(DbPractice::getPractice(practice.rziCode));
	User::setCurrentDoctor(DbDoctor::getDoctor(doctor.LPK).value());
	User::refereshPracticeDoctor();
	NetworkManager::setTimeout(practice.settings.timeout);
	GlobalSettings::setPkcs11PathList(view->getPkcs11List());
	GlobalSettings::setDebug(view->showRequests(), view->showReplies());
	GlobalSettings::setDevBranch(view->devBranch());

	return true;
}
