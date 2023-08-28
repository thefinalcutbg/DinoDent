#pragma once
#include "PracticeSettingsPresenter.h"

#include "View/Interfaces/ISettingsDialog.h"
#include "Network/HIS/UpdateMedications.h"

class SettingsMainPresenter
{
	PracticeSettingsPresenter m_practicePresenter;

	UpdateMedications med_update;

	ISettingsDialog* view{nullptr};

	void setUpdateLabels();

public:

	SettingsMainPresenter();

	void setView(ISettingsDialog* view);
	void updateMedications();
	void okPressed();
	bool applyChanges();

};