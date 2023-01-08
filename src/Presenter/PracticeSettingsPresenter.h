#pragma once
#include "Model/User.h"
#include "View/Interfaces/IPacticeSettings.h"
#include "Model/Validators/RziValidaor.h"
#include "Model/Validators/NzokContractValidator.h"
#include "Model/Validators/DateValidator.h"
#include "Model/Validators/BulstatValidator.h"
class PracticeSettingsPresenter
{
	IPracticeSettings* view {nullptr};
	RziValidator m_rziValidator;
	NotEmptyValidator m_notEmptyValidator;
	DateValidator m_dateValidator;
	NzokContractValidator m_contractValidator;
	BulstatValidator m_bulstatValidator;

	std::string initialRzi;

	std::vector<PracticeDoctor> m_doctorsList;

	int m_currentIndex{ -1 };

public:
	PracticeSettingsPresenter(const std::string initialRzi);
	void setView(IPracticeSettings* view);
	Practice getPractice();
	//does not check db for rzi uniqueness
	bool isValid();
	void nzokContractEnabled(bool enabled);
	void vatEnabled(bool enabled);
	void addDoctor();
	void deleteDoctor();
	void indexChanged(int index);
	void setAdminPrivilege(bool admin);
	void setDoctorNhifSpecialty(NhifSpecialty spec);
	std::vector<PracticeDoctor> getDoctorsList();
};