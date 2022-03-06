#pragma once

#include "View/SettingsDialog/PracticeSettings/IPacticeSettings.h"
#include "View/SettingsDialog/PracticeDoctorSettings/IPracticeDoctorSettings.h"

class IAddPracticeDialog
{
public:
	virtual IPracticeSettings* practiceSettingsView() = 0;
	virtual IPracticeDoctorSettings* doctorSettingsView() = 0;
	virtual void closeDialog() = 0;
};