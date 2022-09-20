#pragma once

#include "View/Interfaces/IPacticeSettings.h"
#include "View/Interfaces/IPracticeDoctorSettings.h"

class IAddPracticeDialog
{
public:
	virtual IPracticeSettings* practiceSettingsView() = 0;
	virtual IPracticeDoctorSettings* doctorSettingsView() = 0;
	virtual void closeDialog() = 0;
};