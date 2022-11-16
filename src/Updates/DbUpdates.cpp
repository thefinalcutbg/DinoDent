#include "DbUpdates.h"
#include "Database/Database.h"
#include "JsonCpp/json.h"
#include "Model/Dental/NhifSheetData.h"
#include <QDebug>
#include "View/Widgets/UpdateDialog.h"
#include "View/ModalDialogBuilder.h"

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


void DbUpdates::update6(UpdateDialog& dialogProgress)
{
	if (Db::version() != 5) {
		ModalDialogBuilder::showMessage(std::to_string(Db::version()));
		return;
	}

	Db::showErrorDialog(true);

	//a function to convert material string to integer
	auto materialStrToIdx = [](const std::string& material, bool restoration = true)
	{
		if (material.empty()) return 0;

		if (restoration) {
			if (material == "Фотополимер") return 1;
			if (material == "Химиополимер") return 2;
			if (material == "ГЙЦ") return 4;
			if (material == "Компомер") return 6;
		}
		else
		{

			//crown case
			if (material == "Металокерамика") return 1;
			if (material == "Пластмаса") return 6;
			if (material == "Цирконий") return 5;
		}

		return 0;
	};



	Db db;

	//separating diagnosis, name and metadata from procedure

	struct ProcedureUpdate {
		long long id;
		std::string diagnosis;
		std::string name;
		std::string data;
	};

	std::vector<ProcedureUpdate> procedureUpdate;

	db.newStatement("SELECT rowid, type, data FROM procedure");

	while (db.hasRows()) {

		int type = db.asInt(1);

		Json::Value data;
		Json::Reader().parse(db.asString(2), data);

		switch (type)
		{
			case 2: {
				{
					if (data["post"].asBool()) data["post"] = 0;
					else data.removeMember("post");
				}

				data["material"] = materialStrToIdx(data["material"].asString());

				break;
			}
			case 5: {
				data["material"] = materialStrToIdx(data["material"].asString(), false);
				break;
			}
			case 6: {
				data.removeMember("system");
				break;
			}
			case 7: {
				data["material"] = materialStrToIdx(data["material"].asString(), false);
				break;
			}
			case 8: {
				data["material"] = 0;
				break;
			}
		}
	

		procedureUpdate.push_back(ProcedureUpdate{
			.id = db.asRowId(0),
			.diagnosis = data.removeMember("diagnosis").asString(),
			.name = data.removeMember("name").asString(),
			.data = Json::FastWriter().write(data)
		});
		/*
		qDebug() << procedureUpdate.back().name.c_str()
			<< procedureUpdate.back().diagnosis.c_str()
			<< procedureUpdate.back().data.c_str();
			*/
	}

	//getting spec data from amblist

	struct AmbSheetUpdate { 

		long long rowid; 
		bool nhif; 
		int spec;
		std::string data;

	};

	std::vector<AmbSheetUpdate> ambSheetUpdate;


	db.newStatement("SELECT amblist.rowid, sum(procedure.nzok) > 0, amblist.nhif, status "
		"FROM amblist JOIN procedure on amblist.rowid = procedure.amblist_rowid "
		"GROUP BY amblist.rowid ");


	while (db.hasRows())
	{
		bool nhif = db.asBool(1);

		Json::Value jsonStatus;
		Json::Reader().parse(db.asString(3), jsonStatus);

		if (jsonStatus.isMember("Obturation")) {
			for (auto& obtur : jsonStatus["Obturation"]) {
				
				obtur["material"] = materialStrToIdx(obtur["material"].asString());
			}
		}

		if (jsonStatus.isMember("Crown")) {
			for (auto& crown : jsonStatus["Crown"]) {
				crown["material"] = materialStrToIdx(crown["material"].asString(), false);
			}
		}

		if (jsonStatus.isMember("Bridge")) {
			for (auto& bridge : jsonStatus["Bridge"]) {
				bridge["material"] = Json::Int(materialStrToIdx(bridge["material"].asString(), false));
			}
		}

		if (jsonStatus.isMember("Splint")) {
			for (auto& splint : jsonStatus["Splint"]) {
				splint["material"] = Json::Int(0);
			}
		}

		if (jsonStatus.isMember("Post")) {
			for (auto& post : jsonStatus["Post"]) {
				post["type"] = 0;
			}
		}

		if (jsonStatus.isMember("Implant")) {
			for (auto& implant : jsonStatus["Implant"])
				implant.removeMember("system");
		}
		
		Json::Value nhifData;

		Json::Reader().parse(db.asString(2), nhifData);


		ambSheetUpdate.push_back(AmbSheetUpdate{
				db.asRowId(0), nhif, 0, Json::FastWriter().write(jsonStatus)
		});
	}

	//THE UPDATE STARTS HERE:

	db.execute("PRAGMA foreign_keys = 0");
	db.execute("BEGIN TRANSACTION");
	db.execute("ALTER TABLE procedure ADD diagnosis VARCHAR");
	db.execute("ALTER TABLE procedure ADD name VARCHAR");
	db.execute("CREATE TABLE sqlitestudio_temp_table AS SELECT* FROM amblist");
	db.execute("DROP TABLE amblist");
	db.execute("CREATE TABLE amblist(rowid INTEGER NOT NULL PRIMARY KEY, patient_rowid INTEGER NOT NULL, date TEXT NOT NULL DEFAULT('1900-01-01T00:00:00'), num INT NOT NULL, lpk VARCHAR(9)  NOT NULL REFERENCES doctor(lpk) ON UPDATE CASCADE, nhif_spec     INT, rzi VARCHAR(10) REFERENCES practice(rzi) ON UPDATE CASCADE NOT NULL, status VARCHAR, FOREIGN KEY(patient_rowid) REFERENCES patient(rowid) ON DELETE CASCADE ON UPDATE CASCADE)");
	db.execute("INSERT INTO amblist(rowid, patient_rowid, date, num, lpk, nhif_spec, rzi, status) SELECT rowid, patient_rowid, date, num, lpk, nhif, rzi, status FROM sqlitestudio_temp_table");
	db.execute("DROP TABLE sqlitestudio_temp_table");

	dialogProgress.setRange(procedureUpdate.size());

	for (auto& p : procedureUpdate) {
		db.newStatement("UPDATE procedure SET diagnosis=?, name=?, data=? WHERE rowid=?");
		db.bind(1, p.diagnosis);
		db.bind(2, p.name);
		db.bind(3, p.data);
		db.bind(4, p.id);
		db.execute();

		dialogProgress.increment();
	}

	dialogProgress.setRange(ambSheetUpdate.size());

	for (auto& sheet : ambSheetUpdate)
	{

		db.newStatement("UPDATE amblist SET nhif_spec = ?, status = ? WHERE rowid=?");

		sheet.nhif ? db.bind(1, sheet.spec) : db.bindNull(1);
		
		db.bind(2, sheet.data);
		db.bind(3, sheet.rowid);

		db.execute();

		dialogProgress.increment();
	}

	db.execute("COMMIT");

	db.execute("PRAGMA foreign_keys = 1");

	db.execute("PRAGMA user_version = 6");
}
