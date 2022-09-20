#pragma once
#include "Model/Dental/ProcedureTemplate.h"
#include <vector>

class PriceListSettingsPresenter;

class IPriceListSettings
{
public:
	virtual void setPresenter(PriceListSettingsPresenter* presenter) = 0;
	virtual void setTemplates(const std::vector<ProcedureTemplate>& templates) = 0;
	virtual void setCurrentIndex(int idx) = 0;
};