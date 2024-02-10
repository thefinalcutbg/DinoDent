#pragma once

#include "View/Interfaces/IPacticeSettings.h"
#include "Model/Settings.h"
#include "Model/DynamicNum.h"

class MainSettingsPresenter;

enum class SettingsTab {General, Practice, PriceList};

class ISettingsDialog
{
public:

	virtual void focusTab(SettingsTab tab) = 0;
	virtual void disableTab(SettingsTab tab) = 0;
	virtual void closeDialog() = 0;
	virtual IPracticeSettings* practiceView() = 0;
	virtual void setSettings(const Settings& settings) = 0;
	virtual Settings getSettings() = 0;
	virtual void setUpdateDate(DynamicNum num, const Date& date) = 0;
	virtual void setPkcs11List(const std::vector<std::string>& list) = 0;
	virtual std::vector<std::string> getPkcs11List() = 0;
	virtual void setDebug(bool showRequests, bool showReplies) = 0;
	virtual bool showRequests() = 0;
	virtual bool showReplies() = 0;
};