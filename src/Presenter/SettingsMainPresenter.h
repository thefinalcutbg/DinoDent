#pragma once
#include "PracticeSettingsPresenter.h"
#include "PriceListSettingsPresenter.h"
#include "PracticeDoctorSettingsPresenter.h"
#include "View/Interfaces/ISettingsDialog.h"
#include "Network/HIS/UpdateMedications.h"

class SettingsMainPresenter
{
	PracticeSettingsPresenter m_practicePresenter;
	PriceListSettingsPresenter m_priceListPresenter;
	PracticeDoctorSettingsPresenter m_practiceDoctorPresenter;

	UpdateMedications med_update;

	ISettingsDialog* view{nullptr};

public:

	SettingsMainPresenter();

	void setView(ISettingsDialog* view);
	void updateMedications() { med_update.update(); };
	void okPressed();
	bool applyChanges();

};