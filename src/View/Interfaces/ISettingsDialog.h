#pragma once

#include "Model/Settings.h"
#include "Model/DynamicNum.h"
#include "Model/UserStructs.h"
#include "Model/GlobalSettingsData.h";

struct PlainTable;
class ProcedureListView;

enum class SettingsTab {General, PKCS, PenTablet, Doctor, Practice, Company, NhifContract, CodeList, SQL};

class ISettingsDialog
{
public:


	virtual void focusTab(SettingsTab tab) = 0;
	virtual void disableTab(SettingsTab tab) = 0;
	virtual void closeDialog() = 0;

	virtual bool allFieldsAreValid() = 0;

	virtual void setSettings(const Settings& settings) = 0;
	virtual void setPractice(const Practice& practice) = 0;
	virtual void setDoctor(const Doctor& doctor) = 0;

	virtual void setDoctorList(const std::vector<PracticeDoctor>& doctors) = 0;
	virtual void setDoctorProperties(bool admin, NhifSpecialty specialty) = 0;
	virtual void setUpdateDate(DynamicNum num, const Date& date) = 0;

	virtual void setGlobalSettings(const GlobalSettingsData& data) = 0;
	virtual GlobalSettingsData getGlobalSettings() = 0;

	virtual Settings getSettings() = 0;
	virtual Practice getPractice() = 0;
	virtual Doctor getDoctor() = 0;


	virtual void replaceCurrentItem(const PracticeDoctor& item) = 0;

	virtual ProcedureListView* getPriceListView() = 0;

	virtual void setSqlTable(const PlainTable& table) = 0;
};
