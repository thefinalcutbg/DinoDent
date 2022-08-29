#include "DbUpdates.h"

#include <unordered_map>
#include <utility>
#include <JsonCpp/json.h>

#include "Database/Database.h"
#include "Model/Date.h"
#include "Model/Parser.h"
#include "Model/Dental/ToothContainer.h"
#include "Model/Financial/Invoice.h"
#include "Database/DbInvoice.h"
#include "View/Widgets/UpdateDialog.h"
#include "Resources.h"


Date parseDate(const std::string& date) {
	return Date(
		stoi(date.substr(0, 2)),
		stoi(date.substr(3, 2)),
		stoi(date.substr(6, 4))
	);
}

void DbUpdates::update1(UpdateDialog* d)
{
	constexpr int version = 1;

	//UPDATING ekatte:
	Db::crudQuery("ALTER TABLE patient ADD COLUMN ekatte INT NOT NULL DEFAULT 4047");

	{
		Json::Value cities = Json::arrayValue;

		Json::Reader reader;
		reader.parse(Resources::fromPath(":/updates/update_dbEkatte.json"), cities);

		d->setRange(cities.size());

		Db db;

		for (auto& pair : cities)
		{
			std::string query("UPDATE patient SET ekatte=" + pair["ekatte"].asString() +
				" WHERE city=" + pair["db"].asString()
			);

			db.execute(query);

			d->increment();
		}


	}

	//updating all DATES:

	//patient birth:
	{

		std::vector<std::pair<long long, Date>> memory;

		for (Db db("SELECT rowid, birth FROM PATIENT"); db.hasRows();)
			memory.push_back(std::make_pair(db.asRowId(0), Date(db.asString(1))));

		d->setRange(memory.size());
		
		for (auto &pair : memory)
		{
			Db::crudQuery("UPDATE patient SET birth='" + pair.second.to8601() + "' "
				"WHERE rowid = " + std::to_string(pair.first));

			d->increment();
		}
	}

	{
		Db db;
		
		db.execute("ALTER TABLE amblist RENAME COLUMN status_json TO status");
		db.execute("ALTER TABLE amblist ADD COLUMN nhif VARCHAR");
	}

	{
		d->setRange(0);

		std::vector<std::pair<long long, std::string>> memory;
	
		for (Db db("SELECT rowid, status FROM amblist"); db.hasRows();)
		{	
			ToothContainer t;

			Parser::parse(db.asString(1), t);

			memory.push_back(std::make_pair(db.asRowId(0), Parser::write(t)));
		}

		d->setRange(memory.size());

		for (auto& pair : memory)
		{
			d->increment();

			Db::crudQuery("UPDATE amblist SET status='" + pair.second + "' "
				"WHERE rowid = " + std::to_string(pair.first));
		}
	}
	
	{
		std::vector<std::pair<long long, std::string>> memory;

		d->setRange(0);

		for (Db db("SELECT rowid, data FROM financial WHERE month_notif = 0"); 
			 db.hasRows();
		)
		{
			Invoice inv;
			Parser::parse(db.asString(1), inv);
			memory.push_back(std::make_pair(db.asRowId(0), Parser::write(inv)));
		}

		d->setRange(memory.size());

		for (auto& pair : memory)
		{
			d->increment();

			std::string updateQuery{
				"UPDATE financial SET data='" + pair.second + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}
	}
	


	{

		Db::crudQuery("ALTER TABLE ambList ADD COLUMN date TEXT NOT NULL DEFAULT '1900-01-01T00:00:00'");
		
		std::vector<std::pair<long long, Date>> memory;

		std::string query{
			"SELECT rowid, day, month, year FROM ambList"
		};

		d->setRange(0);

		for (Db db(query); db.hasRows();)
		{

			memory.push_back(std::make_pair(db.asRowId(0), Date(db.asInt(1), db.asInt(2), db.asInt(3))));
		}

		d->setRange(memory.size());

		for (auto& pair : memory)
		{
			d->increment();

			std::string updateQuery{
				"UPDATE ambList SET date='" + pair.second.to8601() + "T00:00:00" "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}

	}

	{
		Db::crudQuery("ALTER TABLE procedure ADD COLUMN date TEXT NOT NULL DEFAULT '1900-01-01'");

		std::vector<std::pair<long long, Date>> memory;

		std::string query{
			"SELECT procedure.rowid, procedure.day, amblist.month, amblist.year FROM procedure LEFT JOIN ambList ON procedure.amblist_rowid = amblist.rowid"
		};

		for (Db db(query); db.hasRows();)
		{

			memory.push_back(std::make_pair(db.asRowId(0), Date(db.asInt(1), db.asInt(2), db.asInt(3))));
		}

		d->setRange(memory.size());

		for (auto& pair : memory)
		{
			d->increment();

			std::string updateQuery{
				"UPDATE procedure SET date='" + pair.second.to8601() + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}

	}
	
	

	Db::crudQuery("ALTER TABLE financial ADD COLUMN date TEXT NOT NULL DEFAULT '1900-01-01'");

	{
		
		std::vector<std::pair<long long, Date>> memory;

		std::string query{
			"SELECT rowid, day, month, year FROM financial"
		};

		d->setRange(0);

		for (Db db(query); db.hasRows();)
		{

			memory.push_back(std::make_pair(db.asRowId(0), Date(db.asInt(1), db.asInt(2), db.asInt(3))));
		}

		d->setRange(memory.size());

		for (auto& pair : memory)
		{
			d->increment();

			std::string updateQuery{
				"UPDATE financial SET date='" + pair.second.to8601() + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}
	}

	//TAX EVENT DATE

	{

		std::vector<std::pair<long long, std::string>> memory;

		std::string query{
			"SELECT rowid, data FROM financial WHERE month_notif = 0"
		};

		d->setRange(0);

		for (Db db(query); db.hasRows();)
		{
			Json::Value json;
			Json::Reader reader;
			reader.parse(db.asString(1), json);

			if (json.isMember("mainDocumentNum")) {
				json["mainDocumentDate"] = Date{json["mainDocumentDate"].asString()}.to8601();
			}

			json["taxEventDate"] = Date{ json["taxEventDate"].asString() }.to8601();
			Json::FastWriter writer;

			memory.push_back(std::make_pair(db.asRowId(0), writer.write(json)));
		}

		d->setRange(memory.size());

		for (auto& pair : memory)
		{
			d->increment();

			std::string updateQuery{
				"UPDATE financial SET data='" + pair.second + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}
	}

	//PERIO STATUS DATE

	{
		Db::crudQuery("ALTER TABLE periostatus ADD COLUMN date TEXT NOT NULL DEFAULT '1900-01-01'");
		std::vector<std::pair<long long, Date>> memory;

		d->setRange(0);

		std::string query{
			"SELECT rowid, day, month, year FROM financial"
		};

		for (Db db(query); db.hasRows();)
		{
			memory.push_back(std::make_pair(db.asRowId(0), Date(db.asInt(1), db.asInt(2), db.asInt(3))));
		}

		d->setRange(memory.size());

		for (auto& pair : memory)
		{
			d->increment();

			std::string updateQuery{
				"UPDATE periostatus SET date='" + pair.second.to8601() + "' "
				"WHERE rowid = " + std::to_string(pair.first)
			};

			Db::crudQuery(updateQuery);
		}

	}


	Db::crudQuery("ALTER TABLE doctor ADD COLUMN phone VARCHAR");
	Db::crudQuery("ALTER TABLE practice_doctor ADD COLUMN specialty INT NOT NULL DEFAULT 1");

		{

		std::unordered_map<std::string, std::optional<NzokContract>> memory;

		

		std::string query{
			"SELECT rzi, nzok_contract FROM practice"
		};

		d->setRange(0);

		for (Db db(query); db.hasRows();)
		{

			memory[db.asString(0)] = Parser::parseContract(db.asString(1));
		}

		d->setRange(memory.size());

		for (auto& pair : memory)
		{
			d->increment();

			std::string updateQuery{
				"UPDATE practice SET nzok_contract='" + Parser::write(pair.second) + "' "
				"WHERE rzi='" + pair.first + "'"
			};

			Db::crudQuery(updateQuery);
		}


	}

	//cleaning up unneeded tables

	Db db;
	db.execute("PRAGMA foreign_keys = OFF");

	db.execute("CREATE TABLE sqlitestudio_temp_table AS SELECT * FROM patient");
	db.execute("DROP TABLE patient");
	db.execute("CREATE TABLE patient ( rowid INTEGER PRIMARY KEY, type INT NOT NULL, id VARCHAR (10) NOT NULL, birth VARCHAR (10) NOT NULL, sex BOOLEAN NOT NULL, fname VARCHAR (50) NOT NULL, mname VARCHAR (50), lname VARCHAR (50) NOT NULL, ekatte INT DEFAULT (4047) NOT NULL, address VARCHAR (100), hirbno VARCHAR (8), phone VARCHAR (20), allergies VARCHAR (400), currentDiseases VARCHAR (400), pastDiseases VARCHAR (400) )");
	db.execute("INSERT INTO patient ( rowid, type, id, birth, sex, fname, mname, lname, address, hirbno, phone, allergies, currentDiseases, pastDiseases, ekatte ) SELECT rowid, type, id, birth, sex, fname, mname, lname, address, hirbno, phone, allergies, currentDiseases, pastDiseases, ekatte FROM sqlitestudio_temp_table");
	db.execute("DROP TABLE sqlitestudio_temp_table");

	db.execute("CREATE TABLE sqlitestudio_temp_table AS SELECT * FROM procedure");
	db.execute("DROP TABLE procedure");
	db.execute("CREATE TABLE procedure (rowid INTEGER NOT NULL PRIMARY KEY, amblist_rowid INTEGER NOT NULL, nzok INT NOT NULL, code VARCHAR (10) NOT NULL, ksmp VARCHAR (8) NOT NULL, type INT NOT NULL, tooth INT NOT NULL, deciduous INT NOT NULL, price REAL NOT NULL, data VARCHAR NOT NULL, date TEXT NOT NULL DEFAULT '1900-01-01', FOREIGN KEY ( amblist_rowid ) REFERENCES amblist (rowid) ON DELETE CASCADE ON UPDATE CASCADE)");
	db.execute("INSERT INTO procedure (rowid, amblist_rowid, nzok, code, ksmp, type, tooth, deciduous, price, data, date ) SELECT rowid, amblist_rowid, nzok, code, ksmp, type, tooth, deciduous, price, data, date FROM sqlitestudio_temp_table");
	db.execute("DROP TABLE sqlitestudio_temp_table");

	db.execute("CREATE TABLE sqlitestudio_temp_table AS SELECT * FROM periostatus");
	db.execute("DROP TABLE periostatus");
	db.execute("CREATE TABLE periostatus (rowid INTEGER NOT NULL PRIMARY KEY, patient_rowid INTEGER NOT NULL REFERENCES patient (rowid) ON DELETE CASCADE ON UPDATE CASCADE, lpk VARCHAR (9) NOT NULL REFERENCES doctor (lpk) ON DELETE CASCADE ON UPDATE CASCADE, rzi VARCHAR (10) NOT NULL REFERENCES practice (rzi) ON DELETE CASCADE ON UPDATE CASCADE, data VARCHAR NOT NULL, date TEXT NOT NULL DEFAULT '1900-01-01', FOREIGN KEY ( patient_rowid ) REFERENCES patient (rowid) ON DELETE CASCADE ON UPDATE CASCADE)");
	db.execute("INSERT INTO periostatus ( rowid, patient_rowid, lpk, rzi, data, date ) SELECT rowid, patient_rowid, lpk, rzi, data, date FROM sqlitestudio_temp_table");
	db.execute("DROP TABLE sqlitestudio_temp_table");

	db.execute("CREATE TABLE sqlitestudio_temp_table AS SELECT * FROM financial");
	db.execute("DROP TABLE financial");
	db.execute("CREATE TABLE financial ( rowid INTEGER PRIMARY KEY, practice_rzi VARCHAR (10) NOT NULL REFERENCES practice (rzi) ON DELETE CASCADE ON UPDATE CASCADE, num BIGINT NOT NULL, type INT NOT NULL, month_notif INT, recipient_id VARCHAR (100), recipient_name, recipient_phone VARCHAR (20), recipient_address, data TEXT NOT NULL, date TEXT NOT NULL DEFAULT '1900-01-01' )");
	db.execute("INSERT INTO financial ( rowid, practice_rzi, num, type, month_notif, recipient_id, recipient_name, recipient_phone, recipient_address, data, date ) SELECT rowid, practice_rzi, num, type, month_notif, recipient_id, recipient_name, recipient_phone, recipient_address, data, date FROM sqlitestudio_temp_table");
	db.execute("DROP TABLE sqlitestudio_temp_table");

	db.execute("CREATE TABLE sqlitestudio_temp_table AS SELECT * FROM doctor");
	db.execute("DROP TABLE doctor");
	db.execute("CREATE TABLE doctor ( lpk VARCHAR (9) NOT NULL PRIMARY KEY, pass VARCHAR NOT NULL, fname VARCHAR (50) NOT NULL, mname VARCHAR NOT NULL, lname VARCHAR (50) NOT NULL, egn VARCHAR (10) NOT NULL, several_rhif INTEGER, phone VARCHAR )");
	db.execute("INSERT INTO doctor ( lpk, pass, fname, mname, lname, egn, several_rhif, phone ) SELECT lpk, pass, fname, mname, lname, egn, several_rhif, phone FROM sqlitestudio_temp_table");
	db.execute("DROP TABLE sqlitestudio_temp_table");

	db.execute("CREATE TABLE sqlitestudio_temp_table AS SELECT * FROM amblist");
	db.execute("DROP TABLE amblist");
	db.execute("CREATE TABLE amblist ( rowid INTEGER NOT NULL PRIMARY KEY, patient_rowid INTEGER NOT NULL, num INT NOT NULL, lpk VARCHAR (9) NOT NULL REFERENCES doctor (lpk) ON DELETE CASCADE ON UPDATE CASCADE, rzi VARCHAR (10) REFERENCES practice (rzi) ON DELETE CASCADE ON UPDATE CASCADE NOT NULL, status VARCHAR, nhif VARCHAR, date TEXT NOT NULL DEFAULT '1900-01-01Z00:00:00', FOREIGN KEY ( patient_rowid ) REFERENCES patient (rowid) ON DELETE CASCADE ON UPDATE CASCADE )");
	db.execute("INSERT INTO amblist ( rowid, patient_rowid, num, lpk, rzi, status, nhif, date ) SELECT rowid, patient_rowid, num, lpk, rzi, status, nhif, date FROM sqlitestudio_temp_table");
	db.execute("DROP TABLE sqlitestudio_temp_table");

	db.execute("PRAGMA foreign_keys = ON");

	Db::setVersion(version);
		
}




