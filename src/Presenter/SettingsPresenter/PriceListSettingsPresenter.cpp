#include "PriceListSettingsPresenter.h"
#include "Model/User/User.h"
#include "View/SettingsDialog/PriceListSettings/IPriceListSettings.h"
#include "View/ModalDialogBuilder.h"
#include <algorithm>

PriceListSettingsPresenter::PriceListSettingsPresenter() :
	m_priceList{User::practice().priceList}
{
}

void PriceListSettingsPresenter::setView(IPriceListSettings* view)
{
	this->view = view;
	view->setPresenter(this);
	view->setTemplates(m_priceList);
}

void PriceListSettingsPresenter::editTemplate()
{
	if (m_currentIndex == -1)
		return;
	
	auto result = ModalDialogBuilder::openProcedureTemplateDialog(&m_priceList[m_currentIndex]);
	
	if (result.has_value())
	{
		m_priceList[m_currentIndex] = result.value();

		std::sort(m_priceList.begin(), m_priceList.end(),
			[](const ProcedureTemplate& i, const ProcedureTemplate& j)
			{
				return i.code < j.code;
			});

		view->setTemplates(m_priceList);
	}


}

void PriceListSettingsPresenter::deleteTemplate()
{
	if (m_currentIndex == -1)
		return;

	if (!ModalDialogBuilder::askDialog(u8"Изтриване на процедурата от ценоразписа?")) {
		return;
	}

	m_priceList.erase(m_priceList.begin() + m_currentIndex);
	view->setTemplates(m_priceList);
}

void PriceListSettingsPresenter::addTemplate()
{
	auto result = ModalDialogBuilder::openProcedureTemplateDialog(nullptr, m_priceList.back().code + 1);

	if (result.has_value())
	{
		m_priceList.push_back(result.value());

		std::sort(m_priceList.begin(), m_priceList.end(),
			[](const ProcedureTemplate& i, const ProcedureTemplate& j)
			{
				return i.code < j.code;
			});

		view->setTemplates(m_priceList);
	}
}

void PriceListSettingsPresenter::setCurrentIndex(int index)
{
	m_currentIndex = index;
}