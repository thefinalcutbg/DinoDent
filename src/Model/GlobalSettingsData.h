#pragma once
#include <vector>
#include <string>

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

struct DbSettings {
	enum class DbType { Sqlite, Rqlite };

	DbType mode{ DbType::Sqlite };
	std::string sqliteFilePath{ "database.db" };
	std::string rqliteUrl{ "http://127.0.0.1:4001" };
	std::string rqliteUsr{};
	std::string rqlitePass{};

	bool operator==(const DbSettings& other) const noexcept {
		return mode == other.mode
			&& sqliteFilePath == other.sqliteFilePath
			&& rqliteUrl == other.rqliteUrl
			&& rqliteUsr == other.rqliteUsr
			&& rqlitePass == other.rqlitePass;
	}

	bool operator!=(const DbSettings& other) const noexcept {
		return !(*this == other);
	}
};
