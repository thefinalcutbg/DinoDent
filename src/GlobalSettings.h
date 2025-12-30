#pragma once
#include <string>
#include <vector>
#include "Model/GlobalSettingsData.h"

namespace GlobalSettings
{
	void createCfgIfNotExists();

	std::string getDbBackupFilepath();
	std::vector<std::string> getDefaultPkcs11Paths();
	std::string telemetryId();

	std::string getDbPath();
	std::string setDbPath();

	bool showRequestsEnabled();
	bool showRepliesEnabled();
    bool showDbDebugEnabled();
	bool devBranch();

	void setSettings(const GlobalSettingsData& settings);
	GlobalSettingsData getSettings();

}
