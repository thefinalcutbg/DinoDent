#pragma once
#include <vector>
#include <string>
#include <optional>

enum DirType {
	PRACTICE, DOCTOR, PATIENTLF, PATIENTFL, YEARMONTH, DOCTYPE
};

struct GlobalSettingsData {
	std::vector<std::string> pkcs11_list;
	bool dev_branch{ false };
	bool show_requests{ false };
	bool show_replies{ false };
    bool show_dbDebug { false };
	int signer_model{ 0 };
	std::string signer_filepath;

	std::string pdfDir{};
	std::vector<DirType> subdirStructure;
};

struct DbServerConfig {
	std::string rqliteUrl{ "http://localhost:4001" };
	std::string rqliteUsr{};
	std::string rqlitePass{};
	std::string clientCertPath;
	std::string clientKeyPath;
	std::string clientKeyPass;
	std::string caCertPath;

	bool operator==(const DbServerConfig& other) const noexcept {
		return rqliteUrl == other.rqliteUrl
			&& rqliteUsr == other.rqliteUsr
			&& rqlitePass == other.rqlitePass
			&& caCertPath == other.caCertPath
			&& clientCertPath == other.clientCertPath
			&& clientKeyPath == other.clientKeyPath
			&& clientKeyPass == other.clientKeyPass;
	}

	bool operator!=(const DbServerConfig& other) const noexcept {
		return !(*this == other);
	}
};

struct DbSettings {
	enum class DbType { Sqlite, Rqlite };

	DbType mode{ DbType::Sqlite };
	std::string sqliteFilePath{ "database.db" };

	DbServerConfig dbServerConfig;

	bool operator==(const DbSettings& other) const noexcept {
		return mode == other.mode
			&& sqliteFilePath == other.sqliteFilePath
			&& dbServerConfig == other.dbServerConfig;
	}

	bool operator!=(const DbSettings& other) const noexcept {
		return !(*this == other);
	}
};
