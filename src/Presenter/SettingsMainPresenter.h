#pragma once
#include "PracticeSettingsPresenter.h"
#include "PriceListSettingsPresenter.h"

#include "View/Interfaces/ISettingsDialog.h"
#include "Network/HIS/UpdateMedications.h"
#include "Network/HIS/UpdateMkb.h"
class SettingsMainPresenter
{
	PracticeSettingsPresenter m_practicePresenter;
	PriceListSettingsPresenter m_priceListPresenter;


	UpdateMedications med_update;
//	UpdateMkb mkb_update;

	ISettingsDialog* view{nullptr};

	void setUpdateLabels();

public:

	SettingsMainPresenter();

	void setView(ISettingsDialog* view);
	void updateMedications();
	void okPressed();
	bool applyChanges();

};