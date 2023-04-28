#pragma once
#include "Model/Dental/ProcedureCode.h"
#include <vector>

class PriceListSettingsPresenter;

class IPriceListSettings
{
public:
	virtual void setPresenter(PriceListSettingsPresenter* presenter) = 0;
	virtual void setTemplates(const std::vector<ProcedureCode>& templates) = 0;
	virtual void setCurrentIndex(int idx) = 0;
};