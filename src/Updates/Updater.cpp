#include "Updater.h"
#include "Database/Database.h"
#include "DbUpdates.h"
#include "View/Widgets/UpdateDialog.h"

constexpr int db_current_version = 5;

void DbUpdater::updateDb()
{

	if (Db::version() == db_current_version) return;

	UpdateDialog d;

	d.execute();

}
