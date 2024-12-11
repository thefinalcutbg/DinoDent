#include "DbUpdates.h"
#include "Database/Database.h"
#include "Resources.h"
#include "GlobalSettings.h"
#include "View/Widgets/UpdateDialog.h"


void DbUpdates::update26(UpdateDialog& d) {

	if (Db::version() != 25) return;

	Db db;

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

	GlobalSettings::setPkcs11PathList(GlobalSettings::getDefaultPkcs11Paths());

}