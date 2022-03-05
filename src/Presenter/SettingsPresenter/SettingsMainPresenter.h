#pragma once
#include "PracticeSettingsPresenter.h"
#include "PriceListSettingsPresenter.h"
#include "PracticeDoctorSettingsPresenter.h"
#include "View/SettingsDialog/ISettingsDialog.h"

class SettingsMainPresenter
{
	PracticeSettingsPresenter m_practicePresenter;
	PriceListSettingsPresenter m_priceListPresenter;
	PracticeDoctorSettingsPresenter m_practiceDoctorPresenter;

	ISettingsDialog* view{nullptr};

public:

	SettingsMainPresenter();

	void setView(ISettingsDialog* view);

	void okPressed();
	bool applyChanges();

};