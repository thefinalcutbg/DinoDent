#pragma once
#include "Model/User/User.h"

class PracticeDoctorSettingsPresenter;

enum class AdminStatus{Checked, Unchecked, Hidden};

class IPracticeDoctorSettings
{
public:
	virtual void setPresenter(PracticeDoctorSettingsPresenter* presenter) = 0;
	virtual void setDoctorList(const std::vector<PracticeDoctor>& doctors) = 0;
	virtual void setAdminCheckbox(AdminStatus status) = 0;
	virtual void replaceCurrentItem(const PracticeDoctor& item) = 0;
};