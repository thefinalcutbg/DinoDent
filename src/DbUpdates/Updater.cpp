#include "Updater.h"
#include "Database/Database.h"
#include "Updater.h"
#include "View/Widgets/UpdateDialog.h"
#include "View/ModalDialogBuilder.h"
#include "src/Version.h"
#include "Resources.h"

void DbUpdates::commonUpdate(int toVersion)
{
	Db db;

	if (db.version() != toVersion - 1) return;

	for (auto& query : Resources::getMigrationScript(toVersion))
	{
		db.execute(query);
	}
}

void DbUpdater::updateDb()
{
	UpdateDialog d;

	d.show();

	DbUpdates::update10(d);
	DbUpdates::update11(d);
	DbUpdates::commonUpdate(12);
	DbUpdates::commonUpdate(13);
	DbUpdates::commonUpdate(14);
	DbUpdates::commonUpdate(15);
	DbUpdates::update16();
	DbUpdates::update17();
	DbUpdates::update18();
	DbUpdates::update19(d);
	DbUpdates::commonUpdate(20);
	DbUpdates::commonUpdate(21);
	DbUpdates::commonUpdate(22);
	DbUpdates::commonUpdate(23);
	DbUpdates::commonUpdate(24);
	DbUpdates::commonUpdate(25);
	DbUpdates::update26(d);
	DbUpdates::commonUpdate(27);
    DbUpdates::commonUpdate(28);
	DbUpdates::commonUpdate(29);
	DbUpdates::commonUpdate(30);
	DbUpdates::commonUpdate(31);
	DbUpdates::commonUpdate(32);
	DbUpdates::commonUpdate(33);
    DbUpdates::commonUpdate(34);
	DbUpdates::commonUpdate(35);
	DbUpdates::update36(d);
    DbUpdates::commonUpdate(37);
	DbUpdates::commonUpdate(38);
}
