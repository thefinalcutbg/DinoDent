#include "DbUpdates.h"
#include "Database/Database.h"
#include "Resources.h"
#include "View/Widgets/UpdateDialog.h"
#include <JsonCpp/json.h>

void DbUpdates::update11(UpdateDialog& d)
{
	if (Db::version() != 10) return;

	Db db;

	db.newStatement("SELECT practice.rzi, doctor.egn FROM practice_doctor JOIN doctor on practice_doctor.doctor_lpk = doctor.lpk JOIN practice on practice_doctor.practice_rzi = practice.rzi WHERE practice.legal_entity = 0");

	struct RziEgn {
		std::string rzi;
		std::string egn;
	};

	std::vector<RziEgn> rzi_egn;

	while (db.hasRows())
	{
		rzi_egn.push_back(
			RziEgn{
				db.asString(0),
				db.asString(1)
			}
		);
	}

	for (auto& query : Resources::getMigrationScript(11))
	{
		db.execute(query);
	}


	for (auto& obj : rzi_egn)
	{
		db.newStatement("UPDATE practice SET self_insured_id=? WHERE rzi=?");
		db.bind(1, obj.egn);
		db.bind(2, obj.rzi);
		db.execute();
	}

	//addint timeout

	struct RowidSettings {
		long long rowid;
		std::string settings;
	};

	std::vector<RowidSettings> rowid_settings;

	db.newStatement("SELECT rowid, settings FROM practice");

	while (db.hasRows())
	{
		rowid_settings.push_back(
			RowidSettings{
				db.asRowId(0),
				db.asString(1)
			}
		);
	}

	Json::Reader reader;
	Json::FastWriter writer;

	for (auto& o : rowid_settings)
	{
		Json::Value settings;

		reader.parse(o.settings, settings);

		settings["timeout"] = 15;

		o.settings = writer.write(settings);

		db.newStatement("UPDATE practice SET settings=? WHERE rowid=?");

		db.bind(1, o.settings);
		db.bind(2, o.rowid);

		db.execute();
	}



}
