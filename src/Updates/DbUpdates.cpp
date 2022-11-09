#include "DbUpdates.h"
#include "Database/Database.h"
#include "JsonCpp/json.h"


#include "Model/Dental/NhifSheetData.h"
#include <QDebug>


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

//pending!
void DbUpdates::update6()
{
	if (Db::version() != 5) return;

	Db db;

	//separating diagnosis, name and metadata from procedure

	struct ProcedureUpdate {
		long long id;
		std::string diagnosis;
		std::string name;
		std::string data;
	};

	std::vector<ProcedureUpdate> procedureUpdate;

	db.newStatement("SELECT rowid, data FROM procedure");

	while (db.hasRows()) {

		Json::Value data;
		Json::Reader().parse(db.asString(1), data);

		procedureUpdate.push_back(ProcedureUpdate{
			db.asRowId(0),
			data.removeMember("diagnosis").asString(),
			data.removeMember("name").asString(),
			Json::FastWriter().write(data)
		});
		/*
		qDebug() << procedureUpdate.back().name.c_str()
			<< procedureUpdate.back().diagnosis.c_str()
			<< procedureUpdate.back().data.c_str();
			*/
	}

	//getting spec data from amblist

	struct AmbSheetUpdate { long long rowid; bool nhif; NhifSpecification spec; };

	std::vector<AmbSheetUpdate> ambSheetUpdate;


	db.newStatement("SELECT amblist.rowid, sum(procedure.nzok) > 0, amblist.nhif "
		"FROM amblist LEFT JOIN procedure on amblist.rowid = procedure.amblist_rowid "
		"GROUP BY amblist.rowid ");


	while (db.hasRows())
	{
		bool nhif = db.asBool(1);

		NhifSpecification spec{ NhifSpecification::PartialCoverage };

		if (nhif) {
			Json::Value jsonSpec;
			Json::Reader reader; 
			if (reader.parse(db.asString(2), jsonSpec)) {
				spec = static_cast<NhifSpecification>(jsonSpec["spec"].asInt());
			}
		}

		ambSheetUpdate.push_back(AmbSheetUpdate{
				db.asRowId(0), nhif, spec
			});
	}

	

}
