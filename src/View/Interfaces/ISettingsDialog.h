#pragma once

#include "Model/Settings.h"
#include "Model/DynamicNum.h"
#include "Model/UserStructs.h"

class MainSettingsPresenter;
struct PlainTable;

enum class SettingsTab {General, PKCS, Doctor, Practice, Company, NhifContract, SQL};

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
	virtual void setDebug(bool showRequests, bool showReplies) = 0;
	virtual void setDoctorList(const std::vector<PracticeDoctor>& doctors) = 0;
	virtual void setDoctorProperties(bool admin, NhifSpecialty specialty) = 0;
	virtual void setUpdateDate(DynamicNum num, const Date& date) = 0;
	virtual void setPkcs11List(const std::vector<std::string>& list) = 0;

	virtual Settings getSettings() = 0;
	virtual Practice getPractice() = 0;
	virtual Doctor getDoctor() = 0;
	virtual std::vector<std::string> getPkcs11List() = 0;
	virtual bool showRequests() = 0;
	virtual bool showReplies() = 0;

	virtual void setDevBranch(bool dev) = 0;
	virtual bool devBranch() = 0;

	virtual void replaceCurrentItem(const PracticeDoctor& item) = 0;

	virtual void setSqlTable(const PlainTable& table) = 0;
};
