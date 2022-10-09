#include "Updater.h"
#include "Database/Database.h"
#include "DbUpdates.h"
#include "View/Widgets/UpdateDialog.h"

void DbUpdater::updateDb()
{

	UpdateDialog d;

	d.show();

	DbUpdates::update1(&d);
    DbUpdates::update2(&d);
	DbUpdates::update3();
	DbUpdates::update4();
	

}

