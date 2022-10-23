#include "Updater.h"
#include "Database/Database.h"
#include "DbUpdates.h"
#include "View/Widgets/UpdateDialog.h"

void DbUpdater::updateDb()
{
	UpdateDialog d;

	d.execute();

}

