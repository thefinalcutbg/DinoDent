#include "DbUpdates.h"
#include "Database/Database.h"
#include "Resources.h"
#include "Model/FreeFunctions.h"
#include "View/Widgets/UpdateDialog.h"
#include "Model/Parser.h"
#include "Model/Dental/ToothContainer.h"

void DbUpdates::update8(UpdateDialog& progressDialog)
{

	if (Db::version() != 7) return;

	auto script = Resources::getMigrationScript(8);

	if (script.empty()) return;

	Db db;

	//db.execute("BEGIN TRANSACTION");

	for (auto& query : script) { db.execute(query); };

	struct Amb {
		long long rowid;
		std::string nrn;
		std::string lrn;
		std::string based_on;
		bool his_updated;
		long long patient_rowid;
		std::string date;
		int num;
		std::string lpk;
		int nhif_spec;
		int nhif_unfav;
		std::string rzi;
		std::string status;
		bool isNhif;
	};

	std::vector<Amb> newAmb;

	ToothContainer teeth;

	db.newStatement("SELECT rowid, patient_rowid, date, num, lpk, nhif_spec, nhif_unfav, rzi, status, nhif_spec IS NOT NULL FROM temp_amb");
	int counter = 0;
	while (db.hasRows())
	{
		counter++;

		newAmb.push_back(
			Amb{
				.rowid = db.asRowId(0),
				.nrn = {},
				.lrn = FreeFn::getUuid(),
				.based_on = {},
				.his_updated = false,
				.patient_rowid = db.asRowId(1),
				.date = db.asString(2),
				.num = db.asInt(3),
				.lpk = db.asString(4),
				.nhif_spec = db.asInt(5),
				.nhif_unfav = db.asInt(6),
				.rzi = db.asString(7),
				.status = db.asString(8),
				.isNhif = db.asBool(9)

			}
		);

		
		teeth = ToothContainer();

		Parser::parse(newAmb.back().status, teeth);

		for (auto& t : teeth)
		{
			if (t.noData()) t.setStatus(StatusCode::Healthy);
		}

		newAmb.back().status = Parser::write(teeth);

	}

	progressDialog.setRange(newAmb.size());

	for (auto& a : newAmb)
	{
		progressDialog.increment();

		db.newStatement("INSERT INTO amblist "
			"(date, nrn, lrn, his_updated, based_on, num, nhif_spec, nhif_unfav, status, patient_rowid, lpk, rzi, rowid) "
			"VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)");

		

		db.bind(1, a.date);
		db.bind(2, a.nrn);
		db.bind(3, a.lrn);
		db.bind(4, a.his_updated);
		db.bind(5, a.based_on);
		db.bind(6, a.num);
		a.isNhif ? db.bind(7, a.nhif_spec) : db.bindNull(7);
		a.isNhif ? db.bind(8, a.nhif_unfav) : db.bindNull(8);
		db.bind(9, a.status);
		db.bind(10, a.patient_rowid);
		db.bind(11, a.lpk);
		db.bind(12, a.rzi);
		db.bind(13, a.rowid);

		db.execute();
	}

	db.execute("DROP TABLE temp_amb");

	struct Proc {
		long long rowid;
		long long amb_rowid;
		std::string date;
		int financing;
		std::string code;
		int tooth;
		int deciduous;
		std::string data;
		int diagnosis;
		std::string additional;
		int hyperdontic;
		std::string notes;
		int his_index;
		int removed;
	};

	auto getCode = [](int code, int type, bool nhif)->std::string
	{
		std::string result;

		if (nhif)
		{
			switch (code)
			{
				case 101: return "D-01-001";
				case 103: return "D-01-003";
				case 301: return "D-02-001";
				case 508: return "D-04-001";
				case 509: return "D-04-002";
				case 332: return "D-03-001";
				case 333: return "D-03-002";
				case 832: return "D-09-002";
				case 833: return "D-09-003";
				default: return {};
				
			}
		}

		switch (type)
		{
			case 0: return {}; //general
			case 1: return {}; //tooth general
			case 2: return "D-02-001";
			case 3: return "D-04-002";
			case 4: return "D-03-002";
			case 5: return "D-05-001";
			case 6: return "D-06-002";
			case 7: return "D-07-001";
			case 8: return "D-08-001";
			case 9: return "D-05-003";
			case 10: return "D-03-003";
			default: return {};
		}
	};

	std::vector<Proc> newProc;

	db.newStatement("SELECT rowid, amblist_rowid, date, financing_source, code, type, tooth, deciduous, data, diagnosis, notes, name FROM temp_proc");

	while (db.hasRows()) {

		Proc p{
				.rowid = db.asRowId(0),
				.amb_rowid = db.asRowId(1),
				.date = db.asString(2),
				.financing = db.asInt(3),
				.code = {}, //get code!
				.tooth = db.asInt(6),
				.deciduous = db.asInt(7),
				.data = db.asString(8),
				.diagnosis = 0,
				.additional = db.asString(9),
				.hyperdontic = 0,
				.notes = db.asString(10),
				.his_index = 0,
				.removed = 0
		};

		bool nhif = db.asInt(3) == 2;
		int intCode = db.asInt(4);
		int type = db.asInt(5);

		std::string name = db.asString(11);

		if (type == 4) { p.notes = name; }

		if (name == "Преглед") { p.code = "D-01-001"; }

		if (nhif)
		{
			if (intCode == 832)
			{
				p.data = "{\"begin\":1,\"end\":14}";

			}
			else if (int code = 833)
			{
				p.data = "{\"begin\":17,\"end\":30}";
			}
		}

		p.code = getCode(intCode, type, nhif);


		if (p.code.empty()) continue;

		newProc.push_back(p);

		

	}

	progressDialog.setRange(newProc.size());

	for (auto& p : newProc)
	{
		db.newStatement(
			"INSERT INTO procedure "
			"(date, financing_source, code, tooth, deciduous, data, amblist_rowid, diagnosis, notes, hyperdontic, his_index, additional_diagnosis, removed, rowid) "
			"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

		db.bind(1, p.date);
		db.bind(2, p.financing);
		db.bind(3, p.code);
		db.bind(4, p.tooth);
		db.bind(5, p.deciduous);
		db.bind(6, p.data);
		db.bind(7, p.amb_rowid);
		db.bind(8, p.diagnosis);
		db.bind(9, p.notes);
		db.bind(10, p.hyperdontic);
		db.bind(11, p.his_index);
		db.bind(12, p.additional);
		db.bind(13, p.removed);
		db.bind(14, p.rowid);
		db.execute();

		progressDialog.increment();


	}

	db.execute("DROP TABLE temp_proc");


	db.execute("PRAGMA foreign_keys = 1");

	//db.execute("COMMIT TRANSACTION");

}
