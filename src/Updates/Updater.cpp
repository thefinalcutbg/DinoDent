#include "Updater.h"
#include "Database/Database.h"
#include "DbUpdates.h"
#include "View/Widgets/UpdateDialog.h"

constexpr int current_version = 1;



void DbUpdater::updateDb()
{
	int ver = Db::version();

	if (ver == current_version) return;

	UpdateDialog d;

	d.show();

	if (ver == 0) { DbUpdates::update1(&d); }

	

}

