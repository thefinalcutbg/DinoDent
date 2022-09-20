#pragma once
#include "Model/User.h"
#include "View/Interfaces/IPacticeSettings.h"
#include "Model/Validators/RziValidaor.h"
#include "Model/Validators/NzokContractValidator.h"
#include "Model/Validators/DateValidator.h"

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