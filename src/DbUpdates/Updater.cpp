#include "Updater.h"
#include "Database/Database.h"
#include "DbUpdates.h"
#include "View/Widgets/UpdateDialog.h"
#include "Version.h"

void DbUpdater::updateDb()
{

	if (Db::version() == Version::dbVersion()) return;

	UpdateDialog d;

	d.execute();

}
