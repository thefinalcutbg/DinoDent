#include "Updater.h"
#include "Database/Database.h"
#include "DbUpdates.h"
#include "View/Widgets/UpdateDialog.h"
#include "Version.h"

void DbUpdater::updateDb()
{

	if (Db::version() == Version::dbVersion()) return;

	DbUpdates::backupDatabase();
	
	UpdateDialog d;

	d.show();

	DbUpdates::update10(d);
	DbUpdates::update11(d);
	DbUpdates::update12();
	DbUpdates::update13();
	DbUpdates::update14();
	DbUpdates::update15();
	DbUpdates::update16();
	DbUpdates::update17();
	DbUpdates::update18();
	DbUpdates::update19(d);
	DbUpdates::update20();
	DbUpdates::update21();
	DbUpdates::update22();
	DbUpdates::update23();
}
