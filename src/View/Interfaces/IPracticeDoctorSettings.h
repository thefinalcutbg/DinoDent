#pragma once
#include "Model/UserStructs.h"

class PracticeDoctorSettingsPresenter;

class IPracticeDoctorSettings
{
public:
	virtual void setPresenter(PracticeDoctorSettingsPresenter* presenter) = 0;
	virtual void setDoctorList(const std::vector<PracticeDoctor>& doctors) = 0;
	virtual void setDoctorProperties(bool admin, NhifSpecialty specialty) = 0;
	virtual void hideDoctorProperties() = 0;
	virtual void replaceCurrentItem(const PracticeDoctor& item) = 0;
};