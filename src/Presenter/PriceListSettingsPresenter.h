#pragma once
#include "Model/Dental/ProcedureTemplate.h"
#include <vector>

class IPriceListSettings;

class PriceListSettingsPresenter
{
	std::vector<ProcedureTemplate> m_priceList;

	int m_currentIndex{ -1 };

	IPriceListSettings* view{nullptr};

public:
	PriceListSettingsPresenter();
	void setPriceList(const std::vector<ProcedureTemplate>& priceList) { m_priceList = priceList; }
	const std::vector<ProcedureTemplate>& priceList() { return m_priceList; }
	void setView(IPriceListSettings* view);
	void editTemplate();
	void deleteTemplate();
	void addTemplate();
	void setCurrentIndex(int index);
};