#pragma once
#include <string>
#include <vector>

namespace GlobalSettings
{
	void createCfgIfNotExists();

	std::string getDbBackupFilepath();
	std::vector<std::string> getDefaultPkcs11Paths();
	std::string telemetryId();

	bool getDevBranch();
	void setDevBranch(bool dev);

	std::string getDbPath();
	std::string setDbPath();

	const std::vector<std::string>& pkcs11PathList();
	void setPkcs11PathList(const std::vector<std::string> list);

	void setDebug(bool showRequests, bool showReplies);
	bool showRequestsEnabled();
	bool showRepliesEnabled();


}