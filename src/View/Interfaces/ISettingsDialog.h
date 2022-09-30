#pragma once

#include "View/Interfaces/IPacticeSettings.h"
#include "View/Interfaces/IPracticeDoctorSettings.h"
#include "View/Widgets/PriceListSettings.h"
#include "Model/Settings.h"
#include "Model/DynamicNum.h"

class MainSettingsPresenter;

enum class SettingsTab {General, Practice, PriceList, Doctor};

class ISettingsDialog
{
public:

	virtual void focusTab(SettingsTab tab) = 0;
	virtual void closeDialog() = 0;
	virtual IPracticeSettings* practiceView() = 0;
	virtual IPriceListSettings* priceListView() = 0;
	virtual IPracticeDoctorSettings* practiceDoctorView() = 0;
	virtual void setSettings(const Settings& settings) = 0;
	virtual Settings getSettings() = 0;
	virtual void setUpdateDate(DynamicNum num, const Date& date) = 0;
	
};