#pragma once
#include "Model/Dental/ProcedureCode.h"
#include <vector>

class IPriceListSettings;

class PriceListSettingsPresenter
{
	std::vector<ProcedureCode> m_priceList;

	int m_currentIndex{ -1 };

	IPriceListSettings* view{nullptr};

public:
	PriceListSettingsPresenter();
	void setPriceList(const std::vector<ProcedureCode>& priceList) { m_priceList = priceList; }
	const std::vector<ProcedureCode>& priceList() { return m_priceList; }
	void setView(IPriceListSettings* view);
	void deleteTemplate();
	void addTemplate();
	void setCurrentIndex(int index);
};