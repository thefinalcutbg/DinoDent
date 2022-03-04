#include "PriceListSettingsPresenter.h"
#include "Model/User/UserManager.h"
#include "View/SettingsDialog/PriceListSettings/IPriceListSettings.h"
#include "View/ModalDialogBuilder.h"
#include "Database/DbLogin.h"
PriceListSettingsPresenter::PriceListSettingsPresenter() :
	m_priceList{UserManager::currentUser().practice.priceList}
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
	if (m_currentIndex == 1)
		return;
	
	auto result = ModalDialogBuilder::openProcedureTemplateDialog(&m_priceList[m_currentIndex]);
	
	if (result.has_value())
	{
		m_priceList[m_currentIndex] = result.value();
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
	auto result = ModalDialogBuilder::openProcedureTemplateDialog();

	if (result.has_value())
	{
		m_priceList.push_back(result.value());
		view->setTemplates(m_priceList);
	}
}

void PriceListSettingsPresenter::setCurrentIndex(int index)
{
	m_currentIndex = index;
}