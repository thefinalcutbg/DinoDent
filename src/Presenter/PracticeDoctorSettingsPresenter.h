#pragma once
#include "Model/UserStructs.h"
#include "View/Interfaces/IPracticeDoctorSettings.h"


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
	void setDoctorNhifSpecialty(NhifSpecialty spec);
	bool isValid();
	std::vector<PracticeDoctor> getDoctorsList();
};
