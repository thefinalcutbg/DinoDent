#pragma once
#include "Model/User/User.h"
#include "View/SettingsDialog/PracticeSettings/IPacticeSettings.h"
#include "Model/Validator/RziValidaor.h"
#include "Model/Validator/NzokContractValidator.h"
#include "Model/Validator/DateValidator.h"

class PracticeSettingsPresenter
{
	IPracticeSettings* view {nullptr};
	RziValidator m_rziValidator;
	NotEmptyValidator m_notEmptyValidator;
	DateValidator m_dateValidator;
	NzokContractValidator m_contractValidator;

public:
	PracticeSettingsPresenter();
	void setView(IPracticeSettings* view);
	void setPractice(const Practice& practice);
	Practice getPractice();
	bool isValid();
	void nzokContractEnabled(bool enabled);
	void vatEnabled(bool enabled);
};