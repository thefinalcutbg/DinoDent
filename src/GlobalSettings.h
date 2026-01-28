#pragma once
#include <string>
#include <vector>
#include "Model/GlobalSettingsData.h"


namespace GlobalSettings
{
	void createCfgIfNotExists();

	DbSettings getDbSettings();
	void setDbSettings(const DbSettings& s);

	std::string getDbBackupFilepath();
	std::vector<std::string> getDefaultPkcs11Paths();
	std::string telemetryId();

	std::string getNumMedJson();
	void writeNumMedJson(const std::string& numMed);
	
	bool showRequestsEnabled();
	bool showRepliesEnabled();
    bool showDbDebugEnabled();
	bool devBranch();

	void setSettings(const GlobalSettingsData& settings);
	GlobalSettingsData getSettings();

}
