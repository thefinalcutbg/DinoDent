#include "Updater.h"
#include "Database/Database.h"
#include "DbUpdates.h"
#include "View/Widgets/UpdateDialog.h"
#include "GlobalConst.h"

void DbUpdater::updateDb()
{

	if (Db::version() == GlobalConst::db_version) return;

	UpdateDialog d;

	d.execute();

}
