#include "DbUpdates.h"
#include "Database/Database.h"
#include "Resources.h"
#include "View/Widgets/UpdateDialog.h"

void DbUpdates::update11(UpdateDialog& d)
{
	if (Db::version() != 10) return;

	Db db;

	for (auto& query : Resources::getMigrationScript(11))
	{
		db.execute(query);
	}

	d.setRange(1000000);

	for (int i = 0; i < 1000000; i++) {
		d.increment();
	}

}
