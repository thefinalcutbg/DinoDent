#pragma once
#include "Model/User/UserStructs.h"
#include "View/SettingsDialog/PracticeDoctorSettings/IPracticeDoctorSettings.h"


class PracticeDoctorSettingsPresenter
{
	std::vector<PracticeDoctor> m_doctorsList;

	int m_currentIndex{-1};

	IPracticeDoctorSettings* view {nullptr};

public:
	PracticeDoctorSettingsPresenter();
	void setDoctorsList(const std::vector<PracticeDoctor>& doctorsList);
	void setView(IPracticeDoctorSettings* view);
	void addDoctor();
	void deleteDoctor();
	void indexChanged(int index);
	void setAdminPrivilege(bool admin);
	bool isValid();
	std::vector<PracticeDoctor> getDoctorsList();
};

