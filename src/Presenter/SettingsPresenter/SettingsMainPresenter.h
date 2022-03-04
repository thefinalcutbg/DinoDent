#pragma once
#include "PracticeSettingsPresenter.h"
#include "PriceListSettingsPresenter.h"
#include "View/SettingsDialog/ISettingsDialog.h"

class SettingsMainPresenter
{
	PracticeSettingsPresenter m_practicePresenter;
	PriceListSettingsPresenter m_priceListPresenter;

	ISettingsDialog* view{nullptr};

public:

	SettingsMainPresenter();

	void setView(ISettingsDialog* view);

	void okPressed();
	bool applyChanges();

};