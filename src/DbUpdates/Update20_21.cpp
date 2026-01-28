#include "DbUpdates.h"
#include "Database/Database.h"
#include "Resources.h"
#include "GlobalSettings.h"
#include "View/Widgets/UpdateDialog.h"
#include "Model/FreeFunctions.h"

void DbUpdates::update26(UpdateDialog& d) {

	Db db;

	if (db.version() != 25) return;

	for (auto& query : Resources::getMigrationScript(26))
	{
		db.execute(query);
	}

	//remove crown scope migration from tooth to range

	struct RemoveCrownRow { 
		long long rowid;
		int idx;
	};

	std::vector<RemoveCrownRow> rowsToUpdate;
	
	db.newStatement("SELECT rowid, at_tooth_index FROM procedure WHERE code = 'D-05-003'");

	while (db.hasRows()) {
		rowsToUpdate.push_back({ db.asRowId(0), db.asInt(1) });
	}

	d.setRange(rowsToUpdate.size());

	for (auto& row : rowsToUpdate) {

		db.newStatement("UPDATE procedure SET from_tooth_index=?, to_tooth_index=? WHERE rowid=?");
		db.bind(1, row.idx);
		db.bind(2, row.idx);
		db.bind(3, row.rowid);
		db.execute();

		d.increment();
	}

	auto settings = GlobalSettings::getSettings();
	settings.pkcs11_list = (GlobalSettings::getDefaultPkcs11Paths());
	GlobalSettings::setSettings(settings);

}

void DbUpdates::update36(UpdateDialog& d) {

	//converting amblist treatmentEnd from UTC to local time:

	Db db;

	if (db.version() != 35) return;

	db.newStatement("SELECT rowid, treatment_end FROM amblist WHERE treatment_end IS NOT NULL");

	std::vector<std::pair<long long, std::string>> rows;

	while (db.hasRows()) {
		rows.push_back({ db.asRowId(0), db.asString(1) });
	}
	
	d.setRange(rows.size());

	for (auto& row : rows) {
	
		auto localTime = FreeFn::UTCToLocal(row.second);
		db.newStatement("UPDATE amblist SET treatment_end=? WHERE rowid=?");
		db.bind(1, localTime);
		db.bind(2, row.first);
		db.execute();
		d.increment();
	}

	db.newStatement("UPDATE amblist SET treatment_end = date WHERE treatment_end IS NULL");
	db.execute();

	db.setVersion(36);
}