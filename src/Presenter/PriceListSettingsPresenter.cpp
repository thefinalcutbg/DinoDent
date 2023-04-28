#include "PriceListSettingsPresenter.h"
#include "Model/User.h"
#include "View/Interfaces/IPriceListSettings.h"
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

void PriceListSettingsPresenter::deleteTemplate()
{
	if (m_currentIndex == -1)
		return;

	if (!ModalDialogBuilder::askDialog("Изтриване на процедурата от ценоразписа?")) {
		return;
	}

	m_priceList.erase(m_priceList.begin() + m_currentIndex);
	view->setTemplates(m_priceList);
}

void PriceListSettingsPresenter::addTemplate()
{
	auto result = ModalDialogBuilder::procedureCodeDialog();

	if (result.has_value())
	{
		m_priceList.push_back(result.value());

		std::sort(m_priceList.begin(), m_priceList.end(),
			[](const auto& left, const auto& right)
			{
				return left.code() < right.code();
			});

		view->setTemplates(m_priceList);
	}
}

void PriceListSettingsPresenter::setCurrentIndex(int index)
{
	m_currentIndex = index;
}