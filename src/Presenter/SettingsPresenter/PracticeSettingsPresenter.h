#pragma once
#include "Model/User/UserManager.h"
#include "View/SettingsDialog/PracticeSettings/IPacticeSettings.h"


class PracticeSettingsPresenter
{
	IPracticeSettings* view {nullptr};

public:
	PracticeSettingsPresenter();
	void setView(IPracticeSettings* view);
	void applyChanges();
};