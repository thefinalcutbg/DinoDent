#include "DbUpdates.h"
#include "Database/Database.h"

void DbUpdates::update5()
{
	if (Db::version() != 4) return;

	Db db;

	constexpr std::string_view queries[8]
	{
		"PRAGMA foreign_keys = 0",
		"CREATE TABLE sqlitestudio_temp_table AS SELECT * FROM doctor",
		"DROP TABLE doctor",
		"CREATE TABLE doctor(lpk VARCHAR(9) NOT NULL PRIMARY KEY, pass VARCHAR NOT NULL, fname VARCHAR(50) NOT NULL, mname VARCHAR NOT NULL, lname VARCHAR(50) NOT NULL, phone VARCHAR NOT NULL, egn VARCHAR(10) NOT NULL, several_rhif INTEGER, auto_login BOOLEAN NOT NULL DEFAULT(0))",
		"INSERT INTO doctor(lpk,pass,fname,mname,lname,phone,egn,several_rhif) SELECT lpk,pass,fname,mname,lname,phone,egn,several_rhif FROM sqlitestudio_temp_table",
		"DROP TABLE sqlitestudio_temp_table",
		"PRAGMA foreign_keys = 1",
		"PRAGMA user_version = 5"
	};

	for (auto& q : queries) {
		db.execute(q.data());
	}



}