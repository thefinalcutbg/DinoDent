#pragma once

#include "PracticeSettings/IPacticeSettings.h"
#include "PriceListSettings/PriceListSettings.h"

class MainSettingsPresenter;

enum class SettingsTab {Practice, PriceList, Doctor};

class ISettingsDialog
{
public:
	virtual void focusTab(SettingsTab tab) = 0;
	virtual void closeDialog() = 0;
	virtual IPracticeSettings* practiceView() = 0;
	virtual IPriceListSettings* priceListView() = 0;
};