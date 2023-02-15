#include "DbUpdates.h"
#include "Database/Database.h"
#include "JsonCpp/json.h"
#include "Model/Dental/NhifSheetData.h"
#include <QFile>
#include "View/Widgets/UpdateDialog.h"
#include "View/ModalDialogBuilder.h"
#include "Path.h"
#include "Model/FreeFunctions.h"
#include "Resources.h"
void DbUpdates::update6(UpdateDialog& dialogProgress)
{
	if (Db::version() != 5) return;

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

			if (data["surfaces"].empty()) {
				data["surfaces"].append(0);
			}
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
	};

	std::vector<AmbSheetUpdate> ambSheetUpdate;

	db.newStatement("SELECT amblist.rowid, sum(procedure.nzok) > 0, amblist.nhif "
		"FROM amblist JOIN procedure on amblist.rowid = procedure.amblist_rowid "
		"GROUP BY amblist.rowid ");

	while (db.hasRows())
	{
		bool nhif = db.asBool(1);

		Json::Value nhifData;

		Json::Reader().parse(db.asString(2), nhifData);


		ambSheetUpdate.push_back(
			AmbSheetUpdate{
				db.asRowId(0), nhif, 0
			}
		);
	}

	//getting status from amblist:

	struct AmbSheetStatus {
		long long rowid;
		std::string status;
	};

	db.newStatement("SELECT amblist.rowid, status FROM amblist");

	std::vector<AmbSheetStatus> ambSheetStatusUpdate;

	while (db.hasRows())
	{
		Json::Value jsonStatus;
		Json::Reader().parse(db.asString(1), jsonStatus);

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

		ambSheetStatusUpdate.push_back(

			AmbSheetStatus{
				db.asRowId(0),
				Json::FastWriter().write(jsonStatus)
			}

		);
	}

	struct NHIFContractPair {
		std::string rzi;
		std::string contractStr;
	};

	db.newStatement("SELECT rzi, nzok_contract FROM practice");

	std::vector<NHIFContractPair> contracts;

	Json::Reader reader;
	Json::FastWriter writer;

	while (db.hasRows()) {

		Json::Value jsonContract;

		reader.parse(db.asString(1), jsonContract);

		if (jsonContract.empty()) continue;

		jsonContract["unfav"] = false;

		contracts.push_back(
			NHIFContractPair{
				.rzi = db.asString(0),
				.contractStr = writer.write(jsonContract)
			}
		);

	}


	//THE UPDATE STARTS HERE:

	db.execute("PRAGMA foreign_keys = 0");
	db.execute("BEGIN TRANSACTION");
	db.execute("ALTER TABLE procedure ADD diagnosis VARCHAR");
	db.execute("ALTER TABLE procedure ADD name VARCHAR");
	db.execute("CREATE TABLE sqlitestudio_temp_table AS SELECT* FROM amblist");
	db.execute("DROP TABLE amblist");
	db.execute("CREATE TABLE amblist(rowid INTEGER NOT NULL PRIMARY KEY, patient_rowid INTEGER NOT NULL, date TEXT NOT NULL DEFAULT('1900-01-01T00:00:00'), num INT NOT NULL, lpk VARCHAR(9)  NOT NULL REFERENCES doctor(lpk) ON UPDATE CASCADE, nhif_spec INT, nhif_unfav INT, rzi VARCHAR(10) REFERENCES practice(rzi) ON UPDATE CASCADE NOT NULL, status VARCHAR, FOREIGN KEY(patient_rowid) REFERENCES patient(rowid) ON DELETE CASCADE ON UPDATE CASCADE)");
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

		db.newStatement("UPDATE amblist SET nhif_spec = ? WHERE rowid=?");

		sheet.nhif ? db.bind(1, sheet.spec) : db.bindNull(1);


		db.bind(2, sheet.rowid);

		db.execute();

		dialogProgress.increment();
	}

	dialogProgress.setRange(ambSheetStatusUpdate.size());

	for (auto& sheet : ambSheetStatusUpdate)
	{
		db.newStatement("UPDATE amblist SET status = ? WHERE rowid=?");

		db.bind(1, sheet.status);
		db.bind(2, sheet.rowid);

		db.execute();

		dialogProgress.increment();
	}

	for (auto& c : contracts)
	{
		db.newStatement("UPDATE practice SET nzok_contract = ? WHERE rzi=?");
		db.bind(1, c.contractStr);
		db.bind(2, c.rzi);

		db.execute();

	}

	db.execute("COMMIT");

	db.execute("CREATE TABLE referral(rowid INTEGER PRIMARY KEY NOT NULL, amblist_rowid INTEGER REFERENCES amblist(rowid) ON DELETE CASCADE ON UPDATE CASCADE NOT NULL, type INTEGER NOT NULL, number INTEGER, nrn TEXT, lrn TEXT, date TEXT NOT NULL, reason INTEGER NOT NULL, diag_main TEXT NOT NULL, diag_add TEXT, comorb_main TEXT, comorb_add TEXT, tooth INTEGER, reason_119 INTEGER, motives_119 TEXT)");

	db.execute("PRAGMA foreign_keys = 1");

	db.execute("VACUUM");

	db.execute("PRAGMA user_version = 6");
}
