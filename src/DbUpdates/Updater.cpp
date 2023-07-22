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

	DbUpdates::update5();
	DbUpdates::update6(d);
	DbUpdates::update7(d);
	DbUpdates::update8(d);
	DbUpdates::update9(d);
	DbUpdates::update10(d);
	DbUpdates::update11(d);
	DbUpdates::update12();
	DbUpdates::update13();
	DbUpdates::update14();


}
